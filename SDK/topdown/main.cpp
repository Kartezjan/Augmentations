#pragma once
#include "stdafx.h"
#include "../../augmentations.h"
#include "../../window_framework/window.h"

#include "entity_system/world.h"

#include "systems/physics_system.h"
#include "systems/steering_system.h"
#include "systems/movement_system.h"
#include "systems/ai_system.h"
#include "systems/animation_system.h"
#include "systems/camera_system.h"
#include "systems/render_system.h"
#include "systems/input_system.h"
#include "systems/gun_system.h"
#include "systems/crosshair_system.h"
#include "systems/lookat_system.h"
#include "systems/chase_system.h"
#include "systems/damage_system.h"
#include "systems/health_system.h"
#include "systems/destroy_system.h"
#include "systems/particle_group_system.h"
#include "systems/particle_emitter_system.h"
#include "systems/script_system.h"

#include "messages/destroy_message.h"
#include "messages/collision_message.h"
#include "messages/intent_message.h"
#include "messages/animate_message.h"
#include "messages/particle_burst_message.h"
#include "messages/damage_message.h"
#include "messages/steering_message.h"

#include "game/body_helper.h"
#include "game/texture_helper.h"

#include "resources/render_info.h"
#include "resources/animate_info.h"
#include "resources/scriptable_info.h"
//separate events and processing passes in systems to enable script callbacks
// or we can do with polluted data/instruction caches
using namespace augmentations;
using namespace entity_system;
using namespace messages;

int main() {
	augmentations::init();

	script_system scripts;
	resources::script::script_reloader.report_errors = &std::cout;
	resources::script::lua_state = scripts.lua_state;
	resources::script::dofile("scripts\\config.lua");

	window::glwindow gl;
	gl.create(scripts.lua_state, rects::wh(100, 100));
	gl.set_show(gl.SHOW);
	window::cursor(false);

	bool quit_flag = false;

	world my_world;

	input_system input(gl, quit_flag);
	steering_system steering;
	movement_system movement;
	animation_system animations;
	crosshair_system crosshairs;
	lookat_system lookat;
	physics_system physics;
	ai_system ai;
	gun_system guns;
	particle_group_system particles;
	particle_emitter_system emitters;
	render_system render(gl);
	camera_system camera(render);
	chase_system chase;
	damage_system damage;
	health_system health;
	destroy_system destroy;

	my_world.add_system(&scripts);
	my_world.add_system(&input);
	my_world.add_system(&steering);
	my_world.add_system(&movement);
	physics.substepping_systems.push_back(&steering);
	physics.substepping_systems.push_back(&movement);
	my_world.add_system(&physics);
	my_world.add_system(&lookat);
	my_world.add_system(&chase);
	my_world.add_system(&crosshairs);
	my_world.add_system(&guns);
	my_world.add_system(&damage);
	my_world.add_system(&health);
	my_world.add_system(&emitters);
	my_world.add_system(&particles);
	my_world.add_system(&animations);
	my_world.add_system(&ai);
	my_world.add_system(&render);
	my_world.add_system(&destroy);
	my_world.add_system(&camera);

	//my_world.register_message_queue<message>();
	my_world.register_message_queue<intent_message>();
	my_world.register_message_queue<damage_message>();
	my_world.register_message_queue<destroy_message>();
	my_world.register_message_queue<animate_message>();
	my_world.register_message_queue<collision_message>();
	my_world.register_message_queue<particle_burst_message>();

	scripts.global("world", my_world);
	scripts.global("window", gl);
	scripts.global("input_system", input);
	scripts.global("ai_system", ai);
	scripts.global("render_system", render);
	scripts.global("physics_system", physics);

	resources::script::script_reloader.add_directory(L"scripts", true);
	resources::script init_script;

	init_script.associate_filename("scripts\\init.lua");
	init_script.add_reload_dependant(&init_script);
	init_script.call();
	std::cout << std::endl;
	lua_gc(scripts.lua_state, LUA_GCCOLLECT, 0);

	while (!quit_flag) {
		my_world.run();
		
		auto& scripts_reloaded = resources::script::script_reloader.get_script_files_to_reload();

		for (auto& script_to_reload : scripts_reloaded) {
			if (script_to_reload->reload_scene_when_modified) {
				my_world.delete_all_entities(true);
				break;
			}
		}

		for (auto& script_to_reload : scripts_reloaded) {
			script_to_reload->call();
		}

		if (!scripts_reloaded.empty()) {
			std::cout << std::endl;
			lua_gc(scripts.lua_state, LUA_GCCOLLECT, 0);
		}
	}

	augmentations::deinit();
	return 0;
}
