#include "particle_emitter_system.h"
#include "entity_system/world.h"
#include "entity_system/entity.h"

#include "../resources/particle_emitter_info.h"

#include "../components/render_component.h"
#include "../components/position_copying_component.h"
#include "../components/particle_group_component.h"

#include "../messages/particle_burst_message.h"

#include "misc/randval.h"

entity_id particle_emitter_system::create_refreshable_particle_group(world& parent_world) {
	entity_id ent = parent_world.create_entity();
	
	ent->add(components::transform());
	ent->add(components::particle_group()).stream_slots[0].destroy_when_empty = false;
	ent->add(components::position_copying());
	ent->add(components::render());

	return ent;
}

void particle_emitter_system::spawn_particle(
	components::particle_group::stream& group, const vec2& position, float rotation, float spread, const resources::emission& emission) {
	auto new_particle = emission.particle_templates[randval(0u, emission.particle_templates.size() - 1)];
	new_particle.vel = vec2().set_from_degrees(
		randval(rotation - spread, rotation + spread)) *
		randval(emission.velocity);

	rotation = new_particle.vel.degrees();

	new_particle.pos = position + emission.offset;
	new_particle.lifetime_ms = 0.f;
	new_particle.face.size *= randval(emission.size_multiplier);
	new_particle.rotation = randval(rotation - emission.initial_rotation_variation, rotation + emission.initial_rotation_variation);
	new_particle.rotation_speed = randval(emission.angular_velocity);

	auto truncated_lifetime = emission.particle_lifetime_ms;

	if (emission.type == emission.STREAM) {
		float remaining_time = group.stream_max_lifetime_ms - group.stream_lifetime_ms;

		/* if remaining time is less than fade_when_ms_remaining */
		if (group.fade_when_ms_remaining > 0.f && remaining_time < group.fade_when_ms_remaining) {
			/* truncate particle's lifetime to create fading effect */
			float multiplier = remaining_time / group.fade_when_ms_remaining;

			truncated_lifetime.first *= multiplier;
			truncated_lifetime.second *= multiplier;
		}
	}

	new_particle.max_lifetime_ms = randval(truncated_lifetime);

	if (emission.randomize_acceleration) {
		new_particle.acc += vec2().set_from_degrees(
			randval(rotation - spread, rotation + spread)) *
			randval(emission.acceleration);
	}

	group.particles.particles.push_back(new_particle);
}

void particle_emitter_system::consume_events() {
	using namespace components;
	using namespace messages;

	auto events = parent_world.get_message_queue<particle_burst_message>();

	for (auto it : events) {
		resources::particle_effect* emissions = nullptr;

		if (it.type == it.CUSTOM)
			emissions = &it.effect;
		else {
			if (it.subject.alive()) {
				auto* emitter = it.subject->find<particle_emitter>();
				if (emitter && emitter->available_particle_effects) {
					auto emissions_found = emitter->available_particle_effects->get_raw().find(it.type);

					if (emissions_found == emitter->available_particle_effects->get_raw().end()) continue;
					emissions = &(*emissions_found).second;
				}
				else continue;
			}
		}

		if (it.local_transform && it.subject.alive()) {
			it.pos += it.subject->get<components::transform>().pos;
			it.rotation += it.subject->get<components::transform>().rotation;
		}

		std::vector<resources::emission*> only_streams;
		
		for (auto& emission : *emissions) {
			float target_rotation = it.rotation + randval(emission.angular_offset);
			float target_spread = randval(emission.spread_degrees);

			if (emission.type == resources::emission::type::BURST) {
				int burst_amount = randval(emission.particles_per_burst);

				entity_id new_burst_entity = parent_world.create_entity();
				new_burst_entity->add(components::particle_group());
				new_burst_entity->add(components::transform());
				new_burst_entity->add(emission.particle_render_template);

				for (int i = 0; i < burst_amount; ++i)
					spawn_particle(new_burst_entity->get<components::particle_group>().stream_slots[0], it.pos, target_rotation, target_spread, emission);

			}

			else if (emission.type == resources::emission::type::STREAM) {
				only_streams.push_back(&emission);
			}
		}

		if (only_streams.empty()) continue;

		size_t stream_index = 0;

		components::particle_group* target_group = nullptr;
		components::position_copying* target_position_copying = nullptr;
		components::render* target_render = nullptr;
		components::transform* target_transform = nullptr;

		if (it.target_group_to_refresh.alive()) {
			target_group = it.target_group_to_refresh->find<components::particle_group>();
			target_position_copying = it.target_group_to_refresh->find<components::position_copying>();
			target_render = it.target_group_to_refresh->find<components::render>();
			target_transform = it.target_group_to_refresh->find<components::transform>();

			target_group->stream_slots.resize(only_streams.size());
		}

		for (auto& stream : only_streams) {
			if (!it.target_group_to_refresh) {
				entity_id new_stream_entity = parent_world.create_entity();
				target_group = &new_stream_entity->add(components::particle_group());
				target_transform = &new_stream_entity->add(components::transform());
				target_render = &new_stream_entity->add(components::render());

				if (it.subject.alive())
					target_position_copying = &new_stream_entity->add(components::position_copying(it.subject));
			}

			float target_rotation = it.rotation + randval(stream->angular_offset);

			//*target_group = components::particle_group();
			auto& target_stream = target_group->stream_slots[stream_index];

			target_stream.stream_info = *stream;
			target_stream.is_streaming = true;
			target_stream.stream_lifetime_ms = 0.f;
			target_stream.target_spread = randval(stream->spread_degrees);
			target_stream.swing_spread = randval(stream->swing_spread);
			target_stream.swings_per_sec = randval(stream->swings_per_sec);


			target_stream.min_swing_spread = randval(stream->min_swing_spread);
			target_stream.min_swings_per_sec = randval(stream->min_swings_per_sec);
			target_stream.max_swing_spread = randval(stream->max_swing_spread);
			target_stream.max_swings_per_sec = randval(stream->max_swings_per_sec);

			target_stream.stream_max_lifetime_ms = randval(stream->stream_duration_ms);
			target_stream.stream_particles_to_spawn = randval(stream->num_of_particles_to_spawn_initially);
			target_stream.swing_speed_change = randval(stream->swing_speed_change_rate);
			target_stream.swing_spread_change = randval(stream->swing_spread_change_rate);

			target_stream.fade_when_ms_remaining = randval(stream->fade_when_ms_remaining);

			*target_transform = components::transform(it.pos, target_rotation);
			target_group->previous_transform = *target_transform;

			*target_render = stream->particle_render_template;

			if (target_position_copying) {
				auto& subject_transform = it.subject->get<components::transform>();
				*target_position_copying = components::position_copying(it.subject);
				target_position_copying->position_copying_type = components::position_copying::position_copying_type::ORBIT;
				target_position_copying->rotation_offset = target_rotation - subject_transform.rotation;
				target_position_copying->rotation_orbit_offset = (it.pos - subject_transform.pos).rotate(-subject_transform.rotation, vec2(0.f, 0.f));
			}

			if (it.target_group_to_refresh.alive()) {
				++stream_index;
				target_stream.destroy_when_empty = false;
			}
		}
	}
}
