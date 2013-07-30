#pragma once
#include "../components/camera_component.h"
#include "../components/transform_component.h"
#include "render_system.h"

using namespace augmentations;
using namespace entity_system;

class camera_system : public processing_system_templated<components::transform, components::camera> {
	render_system& raw_renderer;

public:
	camera_system(render_system& raw_renderer);

	void process_entities(world&) override;
};