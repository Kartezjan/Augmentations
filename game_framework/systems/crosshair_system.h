#pragma once
#include "entity_system/processing_system.h"

#include "../components/crosshair_component.h"
#include "../components/transform_component.h"
#include "../messages/intent_message.h"

using namespace augs;


class crosshair_system : public processing_system_templated<components::transform, components::crosshair> {

public:
	using processing_system_templated::processing_system_templated;
	
	void generate_crosshair_intents();
	
	/* for replayers*/
	void apply_crosshair_intents_to_base_offsets();
	void apply_base_offsets_to_crosshair_transforms();

	void animate_crosshair_sizes();
};