#pragma once
#include "utility/timer.h"

#include "entity_system/processing_system.h"

#include "../components/animate_component.h"
#include "../components/render_component.h"

using namespace augmentations;
using namespace entity_system;

class animation_system : public processing_system_templated<components::animate, components::render> {
public:
	util::timer animation_timer;
	void process_entities(world&) override;
};