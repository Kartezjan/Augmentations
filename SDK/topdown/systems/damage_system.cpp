#include "damage_system.h"
#include "entity_system/world.h"
#include "entity_system/entity.h"

#include "../messages/particle_burst_message.h"
#include "../messages/collision_message.h"
#include "../messages/destroy_message.h"
#include "../messages/damage_message.h"

void damage_system::process_entities(world& owner) {
	auto events = owner.get_message_queue<messages::collision_message>();

	for (auto it : events) {
		auto* object_a = it.subject->find<components::damage>();
		auto* object_b = it.collider->find<components::damage>();

		auto handle_impact = [&](entity* damager, entity* receiver, components::damage& damage) {
			messages::damage_message damage_msg;
			damage_msg.subject = receiver;
			damage_msg.amount = damage.amount;

			messages::particle_burst_message burst_msg;
			burst_msg.subject = receiver;
			burst_msg.pos = it.point;
			burst_msg.rotation = it.impact_velocity.get_radians();
			burst_msg.type = messages::particle_burst_message::burst_type::BULLET_IMPACT;

			owner.post_message(damage_msg);
			owner.post_message(burst_msg);
			owner.post_message(messages::destroy_message(damager));
		};

		if (object_a) handle_impact(it.subject, it.collider, *object_a);
		if (object_b) handle_impact(it.collider, it.subject, *object_b);
	}

	for (auto it : targets) {
		auto& transform = it->get<components::transform>();
		auto& damage = it->get<components::damage>();
	
		if (damage.max_distance >= 0.f && (damage.starting_point - transform.current.pos).length() >= damage.max_distance) 
			owner.post_message(messages::destroy_message(it));
	}
}