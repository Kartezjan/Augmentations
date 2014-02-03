#include "stdafx.h"
#include "movement_system.h"
#include "entity_system/world.h"
#include "../messages/intent_message.h"
#include "../messages/animate_message.h"

#include "../components/gun_component.h"

/* leave the air drag to simulate top speeds in a sidescroller

*/

using namespace messages;

void movement_system::consume_events(world& owner) {
	auto events = owner.get_message_queue<messages::intent_message>();

	for (auto it : events) {
		switch (it.intent) {
		case intent_message::intent_type::MOVE_FORWARD:
			it.subject->get<components::movement>().moving_forward = it.state_flag;
			break;
		case intent_message::intent_type::MOVE_BACKWARD:
			it.subject->get<components::movement>().moving_backward = it.state_flag;
			break;
		case intent_message::intent_type::MOVE_LEFT:
			it.subject->get<components::movement>().moving_left = it.state_flag;
			break;
		case intent_message::intent_type::MOVE_RIGHT:
			it.subject->get<components::movement>().moving_right = it.state_flag;
			break;
		default: break;
		}
	}
}

void movement_system::substep(world& owner) {
	auto& physics_sys = owner.get_system<physics_system>();

	for (auto it : targets) {
		auto& physics = it->get<components::physics>();
		auto& movement = it->get<components::movement>();

		vec2<> resultant;
		resultant.x = movement.moving_right * movement.input_acceleration.x - movement.moving_left * movement.input_acceleration.x;
		resultant.y = movement.moving_backward * movement.input_acceleration.y - movement.moving_forward * movement.input_acceleration.y;
		
		b2Vec2 vel = physics.body->GetLinearVelocity();

		float ground_angle = 0.f;
		bool was_ground_hit = false;

		if (movement.thrust_parallel_to_ground_length > 0.f) {
			auto out = physics_sys.ray_cast(
				physics.body->GetPosition(),
				physics.body->GetPosition() + vec2<>::from_degrees(movement.axis_rotation_degrees + 90) * movement.thrust_parallel_to_ground_length * PIXELS_TO_METERSf,
				movement.ground_filter, it);

			//assert(out.hit);
			was_ground_hit = out.hit;
			if (was_ground_hit)
				ground_angle = out.normal.get_degrees() + 90;
			//else ground_angle = movement.axis_rotation_degrees;
		}

		//if (std::abs(resultant.x) < b2_epsilon && std::abs(vel.x) > b2_epsilon) {
		if (std::abs(resultant.x) < b2_epsilon && vel.LengthSquared() > 0) {
			physics.body->SetLinearDampingVec(movement.inverse_thrust_brake * PIXELS_TO_METERSf);
			physics.body->SetLinearDampingAngle(was_ground_hit ? ground_angle : movement.axis_rotation_degrees);
			//resultant.x 
			//if (vel.x < b2_epsilon) 
			//	resultant.x = movement.input_acceleration.x;
			//else if (vel.x > b2_epsilon)
			//	resultant.x = -movement.input_acceleration.x;
		}
		else {
			physics.body->SetLinearDampingVec(b2Vec2(0, 0));
		}

		//if (movement.axis_speed_constraint >= 0.f) {
		//	if (vel.x < movement.axis_speed_constraint)
		//	clamp(resultant.x, -movement.axis_speed_constraint, movement.axis_speed_constraint);
		//
		//
		//}

		if (resultant.non_zero()) {
			if (movement.braking_damping >= 0.f)
				physics.body->SetLinearDamping(0.f);
			
			resultant.rotate(was_ground_hit ? ground_angle : movement.axis_rotation_degrees, vec2<>());

			physics.body->ApplyForce(resultant * PIXELS_TO_METERSf * physics.body->GetMass(), physics.body->GetWorldCenter() + (movement.force_offset * PIXELS_TO_METERSf), true);
		}
		else if (movement.braking_damping >= 0.f)
			physics.body->SetLinearDamping(movement.braking_damping);


		float32 speed = vel.Normalize();

		if ((vel.x != 0.f || vel.y != 0.f) && movement.air_resistance > 0.f) 
			physics.body->ApplyForce(movement.air_resistance * speed * speed * -vel, physics.body->GetWorldCenter(), true);
	}
}

void movement_system::process_entities(world& owner) {
	for (auto it : targets) {
		auto& physics = it->get<components::physics>();
		auto& movement = it->get<components::movement>();

		b2Vec2 vel = physics.body->GetLinearVelocity();
		float32 speed = vel.Normalize() * METERS_TO_PIXELSf;

		animate_message msg;

		msg.change_speed = true;
		
		if (movement.max_speed_animation == 0.f) msg.speed_factor = 0.f;
		else msg.speed_factor = speed / movement.max_speed_animation;
		
		msg.change_animation = true;
		msg.preserve_state_if_animation_changes = false;
		msg.message_type = ((speed <= 1.f) ? animate_message::type::STOP : animate_message::type::CONTINUE);
		msg.animation_priority = 0;

		for (auto receiver : movement.animation_receivers) {
			animate_message copy(msg);
			copy.animation_type = animate_message::animation::MOVE;

			auto* gun = receiver.target->find<components::gun>();

			if (gun)
				copy.animation_type = gun->current_swing_direction ? animate_message::animation::MOVE_CW : animate_message::animation::MOVE_CCW;

			copy.subject = receiver.target;

			if (!receiver.stop_at_zero_movement)
				copy.message_type = animate_message::type::CONTINUE;

			owner.post_message(copy);
		}
	}
}