#pragma once
#include <Box2D/Dynamics/b2Fixture.h>

#include "../messages/animation_response_message.h"
#include "math/vec2.h"

namespace components {
	struct movement  {
		struct subscribtion {
			augs::entity_id target;
			bool stop_at_zero_movement;
			subscribtion(augs::entity_id target, bool stop_at_zero_movement = true) :
				target(target), stop_at_zero_movement(stop_at_zero_movement) {}
		};

		void add_animation_receiver(augs::entity_id e, bool stop_at_zero_movement) {
			animation_receivers.push_back(subscribtion(e, stop_at_zero_movement));
		}

		void reset_movement_flags() {
			moving_left = moving_right = moving_forward = moving_backward = walking_enabled = 0;
		}

		bool apply_movement_forces = true;

		std::vector<subscribtion> animation_receivers;
		
		int moving_left = 0, moving_right = 0, moving_forward = 0, moving_backward = 0;
		int walking_enabled = 0;
		
		vec2 input_acceleration_axes;
		float acceleration_length = -1.f;

		vec2 applied_force_offset;

		float non_braking_damping = 0.f;
		float braking_damping = 0.f;

		float standard_linear_damping = 0.f;
		
		bool enable_braking_damping = false;

		bool enable_animation = true;

		float make_inert_for_ms = 0.f;

		/* speed at which the receivers' animation speed multiplier reaches 1.0 */
		float max_speed_animation = 1.f;
	};
}