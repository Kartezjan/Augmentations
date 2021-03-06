#include "driver_system.h"
#include "../messages/trigger_hit_confirmation_message.h"
#include "../messages/collision_message.h"

#include "../components/trigger_component.h"
#include "../components/car_component.h"
#include "../components/input_receiver_component.h"
#include "../components/movement_component.h"
#include "../components/rotation_copying_component.h"
#include "../components/physics_component.h"
#include "../components/force_joint_component.h"

#include "../globals/input_profiles.h"

#include "entity_system/world.h"

void driver_system::assign_drivers_from_triggers() {
	auto& confirmations = parent_world.get_message_queue<messages::trigger_hit_confirmation_message>();

	for (auto& e : confirmations) {
		auto subject_car = e.trigger->get<components::trigger>().entity_to_be_notified;
		auto* maybe_car = subject_car->find<components::car>();

		if (maybe_car && e.trigger == maybe_car->left_wheel_trigger && e.domain == detection_domain::TRIGGER_SWITCHING)
			assign_car_ownership(e.detector_body, subject_car);
	}
}

void driver_system::release_drivers_due_to_ending_contact_with_wheel() {
	auto& contacts = parent_world.get_message_queue<messages::collision_message>();

	for (auto& c : contacts) {
		if (c.sensor_end_contact) {
			auto driver = components::physics::get_owner_body_entity(c.subject);
			auto car = components::physics::get_owner_body_entity(c.collider);

			auto* maybe_driver = driver->find<components::driver>();

			if (maybe_driver) {
				if (maybe_driver->owned_vehicle == car) {
					release_car_ownership(driver);
					driver->get<components::movement>().make_inert_for_ms = 500.f;
				}
			}
		}
	}
}
void driver_system::release_drivers_due_to_requests() {
	auto& intents = parent_world.get_message_queue<messages::intent_message>();

	for (auto& e : intents)
		if (e.intent == intent_type::RELEASE_CAR && e.pressed_flag)
			release_car_ownership(e.subject);
}

bool driver_system::release_car_ownership(augs::entity_id driver) {
	return change_car_ownership(driver, augs::entity_id(), true);
}

bool driver_system::assign_car_ownership(augs::entity_id driver, augs::entity_id car) {
	return change_car_ownership(driver, car, false);
}

bool driver_system::change_car_ownership(augs::entity_id driver_entity, augs::entity_id car_entity, bool lost_ownership) {
	auto& driver = driver_entity->get<components::driver>();

	auto* maybe_rotation_copying = driver_entity->find<components::rotation_copying>();
	auto* maybe_physics = driver_entity->find<components::physics>();
	auto* maybe_movement = driver_entity->find<components::movement>();
	auto& force_joint = driver_entity->get<components::force_joint>();

	if (!lost_ownership) {
		auto& car = car_entity->get<components::car>();

		if (car.current_driver.alive())
			return false;

		driver.owned_vehicle = car_entity;
		car.current_driver = driver_entity;
		force_joint.chased_entity = car.left_wheel_trigger;
		driver_entity->enable(force_joint);

		if (maybe_movement) {
			maybe_movement->reset_movement_flags();
			maybe_movement->enable_braking_damping = false;
			maybe_movement->enable_animation = false;
		}

		if (maybe_rotation_copying && maybe_physics) {
			maybe_rotation_copying->update_value = false;
		}

		if (maybe_physics) {
			maybe_physics->set_transform(car.left_wheel_trigger);
			maybe_physics->set_velocity(vec2(0, 0));
			maybe_physics->set_density(driver.density_while_driving);
		}
	}
	else {
		auto& car = driver.owned_vehicle->get<components::car>();

		driver.owned_vehicle.unset();
		car.current_driver.unset();
		driver_entity->disable(force_joint);

		if (maybe_movement) {
			maybe_movement->reset_movement_flags();
			maybe_movement->enable_braking_damping = true;
			maybe_movement->enable_animation = true;

			maybe_movement->moving_left = car.turning_left;
			maybe_movement->moving_right = car.turning_right;
			maybe_movement->moving_forward = car.accelerating;
			maybe_movement->moving_backward = car.deccelerating;
		}
		
		car.reset_movement_flags();

		if (maybe_rotation_copying && maybe_physics) {
			maybe_rotation_copying->update_value = true;
		}

		if (maybe_physics) {
			maybe_physics->set_density(driver.standard_density);
		}
	}

	// networking the message, since it was successful

	return true;
}
