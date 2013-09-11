#pragma once
#include "entity_system/processing_system.h"

#include "../components/damage_component.h"
#include "../components/physics_component.h"
#include "../components/transform_component.h"

using namespace augmentations;
using namespace entity_system;

class damage_system : public processing_system_templated<components::damage, components::transform, components::physics> {
public:
	void process_entities(world&) override;
};