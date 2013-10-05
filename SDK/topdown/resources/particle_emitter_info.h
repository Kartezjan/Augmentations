#pragma once
#include "utility/map_wrapper.h"
#include "../messages/particle_burst_message.h"
#include "../components/render_component.h"

#include "../resources/render_info.h"

namespace resources {
	struct particle {
		augmentations::vec2<> pos, vel, acc;
		resources::sprite face;
		float rotation;
		float rotation_speed;
		float linear_damping;
		float angular_damping;
		float lifetime_ms, max_lifetime_ms;
		bool should_disappear;
		particle() : face(nullptr), lifetime_ms(0.f), should_disappear(true), rotation(0.f), rotation_speed(0.f) {}
	};

	struct emission {
		enum type {
			BURST,
			STREAM
		} type;

		float spread_degrees;

		std::pair<float, float>
			velocity,
			angular_velocity,
			particles_per_sec,
			stream_duration_ms,
			particle_lifetime_ms,
			size_multiplier,
			acceleration;

		std::pair<unsigned, unsigned>
			particles_per_burst;

		float initial_rotation_variation;
		bool randomize_acceleration;

		augmentations::vec2<> offset;
		float angular_offset;

		std::vector<particle> particle_templates;
		components::render particle_render_template;

		void add_particle_template(const particle& p) {
			particle_templates.push_back(p);
		}

		emission() : acceleration(std::make_pair(0.f, 0.f)), randomize_acceleration(false) {}
	};

	typedef std::vector<emission> particle_effect;
	typedef augmentations::util::map_wrapper <
		messages::particle_burst_message::burst_type, particle_effect
	> particle_emitter_info;
}