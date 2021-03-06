#pragma once
#include "entity_system/entity_id.h"
#include "../components/force_joint_component.h"

struct inventory_slot {
	bool items_need_mounting = false;
	float montage_time_multiplier = 1.f;

	bool only_last_inserted_is_movable = false;

	bool for_categorized_items_only = false;
	unsigned long long category_allowed = 0;

	float space_available = 7;

	/*
	true means that:
	- space is disregarded
	- there may be only one item
	- putting item inside does not deactivate its physics component; it is attached to the container entity instead
	*/
	bool is_attachment_slot = false;

	bool is_holsterable() const {
		return !is_attachment_slot;
	}

	vec2::sticking attachment_sticking_mode;
	components::force_joint attachment_force_joint_def;

	std::vector<augs::entity_id> items_inside;

	std::vector<augs::entity_id> get_mounted_items();

	float calculate_free_space_with_children();
};

