#pragma once
#include "entity_system/processing_system.h"

#include "../components/physics_component.h"
#include "../components/visibility_component.h"
#include "../components/transform_component.h"

using namespace augs;

class visibility_system : public processing_system_templated<components::transform, components::visibility> {
public:
	visibility_system(world&);

	void process_entities();

	int draw_triangle_edges;
	int draw_cast_rays;
	int draw_discontinuities;
	int draw_visible_walls;

	float epsilon_ray_distance_variation;
	float epsilon_distance_vertex_hit;
	float epsilon_threshold_obstacle_hit;
};