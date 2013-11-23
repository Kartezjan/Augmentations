#include "stdafx.h"
#include "pathfinding_system.h"

#include "entity_system/world.h"
#include "entity_system/entity.h"

#include "render_system.h"
#include "physics_system.h"

#include "../resources/render_info.h"
#include "../game/body_helper.h"

pathfinding_system::pathfinding_system() : draw_memorised_walls(false), draw_undiscovered(false),
	epsilon_distance_the_same_vertex(3.f) {}

void pathfinding_system::process_entities(world& owner) {
	/* prepare epsilons to be used later, just to make the notation more clear */
	const float epsilon_distance_visible_point_sq = epsilon_distance_visible_point * epsilon_distance_visible_point;
	const float epsilon_distance_the_same_vertex_sq = epsilon_distance_the_same_vertex * epsilon_distance_the_same_vertex;
	
	/* we'll need a reference to physics system for raycasting */
	physics_system& physics = owner.get_system<physics_system>();
	/* we'll need a reference to render system for debug drawing */
	render_system& render = owner.get_system<render_system>();

	for (auto it : targets) {
		/* get AI data and position of the entity */
		auto& visibility = it->get<components::visibility>();
		auto& pathfinding = it->get<components::pathfinding>();
		auto& transform = it->get<components::transform>().current;
		auto body = it->get<components::physics>().body;

		/* check if we request pathfinding */
		if (!pathfinding.session_stack.empty()) {
			/* get visibility information */
			auto& vision = visibility.get_layer(components::visibility::DYNAMIC_PATHFINDING);
			vision.ignore_discontinuities_shorter_than = pathfinding.session().temporary_ignore_discontinuities_shorter_than;

			for (auto& visible_vertex : vision.vertex_hits) {
				bool this_visible_vertex_is_already_memorised = false;

				for (auto& memorised_discovered : pathfinding.session().discovered_vertices) {
					/* if a discontinuity with the same closer vertex already exists */
					if ((memorised_discovered.location - visible_vertex).length_sq() < epsilon_distance_the_same_vertex_sq) {
						this_visible_vertex_is_already_memorised = true;
						memorised_discovered.location = visible_vertex;
						break;
					}
				}

				if (!this_visible_vertex_is_already_memorised) {
					components::pathfinding::pathfinding_session::navigation_vertex vert;
					vert.location = visible_vertex;
					pathfinding.session().discovered_vertices.push_back(vert);
				}
			}

			/* save all new discontinuities from visibility */
			for (auto& disc : vision.discontinuities) {
				bool this_discontinuity_is_already_memorised = false;

				for (auto& memorised_undiscovered : pathfinding.session().undiscovered_vertices) {
					/* if a discontinuity with the same closer vertex already exists */
					if ((memorised_undiscovered.location - disc.points.first).length_sq() < epsilon_distance_the_same_vertex_sq) {
						this_discontinuity_is_already_memorised = true;
						memorised_undiscovered.location = disc.points.first;
						break;
					}
				}

				for (auto& memorised_discovered : pathfinding.session().discovered_vertices) {
					/* if a discontinuity with the same closer vertex already exists */
					if ((memorised_discovered.location - disc.points.first).length_sq() < epsilon_distance_the_same_vertex_sq) {
						this_discontinuity_is_already_memorised = true;
						memorised_discovered.location = disc.points.first;
						break;
					}
				}

				/* if it is unique, push it */
				if (!this_discontinuity_is_already_memorised) {
					components::pathfinding::pathfinding_session::navigation_vertex vert;
					vert.location = disc.points.first;
					auto associated_edge = vision.edges[disc.edge_index];

					vec2<> offset_direction;

					if (associated_edge.first.compare(vert.location))
						offset_direction = associated_edge.first - associated_edge.second;
					else if (associated_edge.second.compare(vert.location))
						offset_direction = associated_edge.second - associated_edge.first;
					else assert(0);

					offset_direction.normalize();

					/* push the sensor a bit further so the body doesn't stop not seeing another targets */
					vert.sensor = vert.location + offset_direction * pathfinding.target_offset;
					pathfinding.session().undiscovered_vertices.push_back(vert);
				}
			}

			/* mark vertices whose sensors distance from the the body is less than distance_navpoint_hit as visited */

			/* prepare body polygon to test for overlaps */
			b2PolygonShape body_poly;
			auto verts = topdown::get_transformed_shape_verts(*reinterpret_cast<entity*>(body->GetUserData()));
			body_poly.Set(verts.data(), verts.size());

			/* for every undiscovered navigation point */
			auto& undiscs = pathfinding.session().undiscovered_vertices;
			undiscs.erase(std::remove_if(undiscs.begin(), undiscs.end(), [&body, &pathfinding, &body_poly](const components::pathfinding::pathfinding_session::navigation_vertex& nav){
				/* prepare edge shape for sensor to test for overlaps */
				b2EdgeShape sensor_edge;
				sensor_edge.Set(nav.location * PIXELS_TO_METERSf, nav.sensor * PIXELS_TO_METERSf);

				/* prepare null transform, both bodies are already in the same frame of reference */
				b2Transform null_transform(b2Vec2(0.f, 0.f), b2Rot(0.f));

				/* if shortest distance between body and sensor fits in distance_navpoint_hit */
				if (b2TestOverlap(&sensor_edge, 0, &body_poly, 0, null_transform, null_transform, pathfinding.distance_navpoint_hit * PIXELS_TO_METERSf)) {
					/* save this sensor in discovered ones and return true to remove it from the undiscovered */
					pathfinding.session().discovered_vertices.push_back(nav);
					return true;
				}
				return false;
			}), undiscs.end());

			/* now for the actual pathfinding routine */

			/* helpful lambda */
			auto& is_point_visible = [&physics, epsilon_distance_visible_point_sq](vec2<> from, vec2<> point, b2Filter& filter){
				auto line_of_sight = physics.ray_cast_px(from, point, &filter);
				return (!line_of_sight.hit || (line_of_sight.intersection - point).length_sq() < epsilon_distance_visible_point_sq);
			};

			/* we are sure here that session stack has at least 1 session
				we drop secondary sessions whose targets are visible
			*/
			if (pathfinding.session_stack.size() >= 2) {
				for (auto old_session = pathfinding.session_stack.begin(); old_session != pathfinding.session_stack.end(); ++old_session) {
					/*  check if there's a line of sight to any of the old targets
					if there's a line of sight to "navigate_to" it will be visible as target to the newer session
					and we either way also handle the current session's target so nothing is missing here
					*/
					if (body->TestPoint((*old_session).target * PIXELS_TO_METERSf) ||
						is_point_visible(transform.pos, (*old_session).target, vision.filter)) {
							/* if there is, roll back to this session */
							pathfinding.session() = (*old_session);
							
							/* if it is the first session, we don't want to erase it since we still need to reach the target */
							if (old_session == pathfinding.session_stack.begin())
								++old_session;

							/* drop unnecessary sessions */
							pathfinding.session_stack.erase(old_session, pathfinding.session_stack.end());
							break;
					}
				}
			}

			if (pathfinding.session_stack.size() == 1) {
				/* if the target is inside body, it's already found */
				if (body->TestPoint(pathfinding.session().target * PIXELS_TO_METERSf)) {
					/* done, target found */
					pathfinding.clear_pathfinding_info();
					continue;
				}

				/* check if there's a line of sight */
				if (is_point_visible(transform.pos, pathfinding.session().target, vision.filter)) {
					/* if there is, navigate directly to target */
					pathfinding.session().navigate_to = pathfinding.session().target;
					continue;
				}
			}

			/* if it is the last session but there's not line of sight,
				or it is not the last session but it was not dropped from the loop which means there's no line of sight to target,
				pick the best navigation candidate
			*/

			auto& vertices = pathfinding.session().undiscovered_vertices;

			if (draw_undiscovered) {
				for (auto& disc : vertices)
					render.lines.push_back(render_system::debug_line(disc.location, disc.sensor, graphics::pixel_32(0, 127, 255, 255)));

				for (auto& disc : pathfinding.session().discovered_vertices)
					if(disc.sensor.non_zero())
					render.lines.push_back(render_system::debug_line(disc.location, disc.sensor, graphics::pixel_32(0, 255, 0, 255)));
			}

			if (!vertices.empty()) {
				/* find discontinuity that is closest to the target */
				auto& local_minimum_discontinuity = (*std::min_element(vertices.begin(), vertices.end(),
					[&pathfinding, &transform](const components::pathfinding::pathfinding_session::navigation_vertex& a,
					const components::pathfinding::pathfinding_session::navigation_vertex& b) {
						auto dist_a = (a.location - pathfinding.session().target).length_sq() + (a.location - transform.pos).length_sq();
						auto dist_b = (b.location - pathfinding.session().target).length_sq() + (b.location - transform.pos).length_sq();
						return dist_a < dist_b;
				}));

				/* extract the closer vertex, condition to faciliate debug */
				if (local_minimum_discontinuity.sensor != pathfinding.session().navigate_to)
					pathfinding.session().navigate_to = local_minimum_discontinuity.sensor;

				bool rays_hit = false;
				auto& subject_verts = topdown::get_transformed_shape_verts(*it);
				subject_verts.push_back(transform.pos);

				for (auto& subject_vert : subject_verts) {
					if (
						is_point_visible(METERS_TO_PIXELSf*subject_vert, local_minimum_discontinuity.location, vision.filter) ||
						is_point_visible(METERS_TO_PIXELSf*subject_vert, local_minimum_discontinuity.sensor, vision.filter)
						) {
						rays_hit = true;
					}
				}

				/* if we can see it, navigate there */
				if (body->TestPoint(local_minimum_discontinuity.location * PIXELS_TO_METERSf) ||
					body->TestPoint(local_minimum_discontinuity.sensor * PIXELS_TO_METERSf) ||
					rays_hit
					) {
				}
				/* else start new navigation session */
				else {
					if (pathfinding.enable_backtracking) {
						vec2<> new_target = pathfinding.session().navigate_to;
						pathfinding.session_stack.push_back(components::pathfinding::pathfinding_session());
						pathfinding.session().target = new_target;
						pathfinding.session().temporary_ignore_discontinuities_shorter_than = pathfinding.starting_ignore_discontinuities_shorter_than;
					}
				}

			}
			else {
				/* something went wrong, let's begin again */
				//if (pathfinding.session_stack.size() == 1) {
					pathfinding.session().discovered_vertices.clear();
					pathfinding.session().undiscovered_vertices.clear();
					//pathfinding.session().temporary_ignore_discontinuities_shorter_than /= 1.5f;
				//}
				//pathfinding.session_stack.resize(1);
			}
		}
	}
}