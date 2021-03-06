#pragma once
#include "entity_system/entity_id.h"
#include "../shared/inventory_slot_id.h"

namespace messages {
	struct item_slot_transfer_request {
		augs::entity_id item;
		inventory_slot_id target_slot;

		void interrupt_mounting();
	};
}