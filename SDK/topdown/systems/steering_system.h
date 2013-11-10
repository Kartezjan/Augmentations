#pragma once
#include "entity_system/processing_system.h"

#include "../components/steering_component.h"
#include "../components/physics_component.h"
#include "../components/transform_component.h"

using namespace augmentations;
using namespace entity_system;

class steering_system : public processing_system_templated<components::transform, components::physics, components::steering> {
public:
	void substep(world&) override;
	void process_entities(world&) override;
	void process_events(world&) override;

	bool avoid_collisions(vec2<> position, vec2<> velocity, float avoidance_rectangle_width, vec2<> target,
		std::vector <std::pair <augmentations::vec2<>, augmentations::vec2<>>>&, float intervention_time_ms, vec2<>& output, int attempt = 0);

	vec2<> seek(vec2<> position, vec2<> velocity, vec2<> target, float max_speed, float arrival_radius);
	vec2<> flee(vec2<> position, vec2<> velocity, vec2<> target, float max_speed, float flee_radius);
	vec2<> predict_interception(vec2<> position, vec2<> velocity, vec2<> target, vec2<> target_velocity, float max_prediction_ms, bool);
};