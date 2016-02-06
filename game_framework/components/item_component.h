#pragma once

#include "container_component.h"

namespace components {
	struct item {
		unsigned categories_for_slot_compatibility = 0;

		unsigned stackability = 1;
		unsigned space_occupied = 1;
		unsigned transfer_time_ms = 1000;

		unsigned dual_wield_accuracy_loss_percentage = 50;
		unsigned dual_wield_accuracy_loss_multiplier = 1;

		container::slot_id current_slot;
	};
}