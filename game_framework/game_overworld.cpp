#include "game_overworld.h"

#include <signal.h>

#include "window_framework/window.h"
#include "script.h"

#include "game_framework/bind_game_framework_and_augs.h"
#include "game_framework/messages/raw_window_input_message.h"
#include "game_framework/messages/camera_render_request_message.h"

#include "game_framework/systems/input_system.h"
#include "game_framework/systems/render_system.h"
#include "game_framework/systems/gui_system.h"

#include <luabind/luabind.hpp>

#include "log.h"

using namespace std;
using namespace augs;

void SignalHandler(int signal) { throw "Access violation!"; }

game_overworld::game_overworld() 
	: main_game_world(*this) {
	main_game_world.initialize_entity_and_component_pools(50000);
}

void game_overworld::set_scene_builder(std::unique_ptr<scene_builder> builder) {
	current_scene_builder = std::move(builder);
}

void game_overworld::initialize_scene() {
	current_scene_builder->initialize(main_game_world);
	clear_window_inputs_once = true;
}

void game_overworld::call_window_script(std::string filename) {
	lua.global_ptr("global_gl_window", &game_window);

	try {
		if (!lua.dofile(filename))
			lua.debug_response();
	}
	catch (char* e) {
		LOG("Exception thrown! %x", e);
		lua.debug_response();
	}
	catch (...) {
		LOG("Exception thrown!");
		lua.debug_response();
	}

	game_window.initial_gl_calls();
}

#define RENDERING_STEPS_DETERMINISTICALLY_LIKE_LOGIC 0

void game_overworld::main_game_loop() {
	bool quit_flag = false;

	while (!quit_flag) {
		main_game_world.get_message_queue<messages::raw_window_input_message>().clear();

		auto raw_window_inputs = game_window.poll_events();

		if (clear_window_inputs_once) {
			raw_window_inputs.clear();
			clear_window_inputs_once = false;
		}

		for (auto& raw_input : raw_window_inputs) {
			if (raw_input.key_event == window::event::PRESSED &&
				raw_input.key == window::event::keys::ESC) {
				quit_flag = true;
				break;
			}

			messages::raw_window_input_message msg;
			msg.raw_window_input = raw_input;

			if(!main_game_world.get_system<input_system>().is_replaying())
				main_game_world.post_message(msg);
		}

#if RENDERING_STEPS_DETERMINISTICALLY_LIKE_LOGIC
		auto steps_to_perform = delta_timer.count_logic_steps_to_perform();

		while (steps_to_perform--) {
#endif

		game_window.clear();

		assign_frame_time_to_delta_for_drawing_time_systems();
		main_game_world.call_drawing_time_systems();

		consume_camera_render_requests();

		main_game_world.restore_transforms_after_drawing();

		restore_fixed_delta();

#if !RENDERING_STEPS_DETERMINISTICALLY_LIKE_LOGIC
		auto steps_to_perform = delta_timer.count_logic_steps_to_perform();

		while (steps_to_perform--) {
#endif
			renderer::get_current().clear_logic_lines();

			main_game_world.perform_logic_step();
			current_scene_builder->perform_logic_step(main_game_world);
		}
	}
}

void game_overworld::consume_camera_render_requests() {
	auto& requests = main_game_world.get_message_queue<messages::camera_render_request_message>();
	auto& target = renderer::get_current();

	for (auto& r : requests) {
		target.set_viewport(r.state.viewport);
		current_scene_builder->execute_drawcalls_for_camera(r);
		target.draw_debug_info(r.state.visible_world_area, r.state.camera_transform, assets::texture_id::BLANK, main_game_world.get_system<render_system>().targets, view_interpolation_ratio());
		
		main_game_world.get_system<gui_system>().draw_gui_overlays_for_camera_rendering_request(r);
	}
	
	current_scene_builder->drawcalls_after_all_cameras(main_game_world);

	game_window.swap_buffers();
	target.clear_geometry();
}

void game_overworld::configure_scripting() {
	bind_game_framework_and_augs(lua);
	main_game_world.bind_this_to_lua_global(lua, "WORLD");

	signal(SIGSEGV, SignalHandler);
}

