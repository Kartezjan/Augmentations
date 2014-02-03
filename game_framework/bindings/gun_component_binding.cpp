#pragma once
#include "stdafx.h"
#include "bindings.h"

#include "../components/gun_component.h"

namespace bindings {
	luabind::scope _gun_component() {
		
		return
			luabind::class_<gun>("gun_component")
			.def(luabind::constructor<>())
			.def(luabind::constructor<const gun&>())
			.def("transfer_barrel_smoke", &gun::transfer_barrel_smoke)
			.def("get_barrel_smoke", &gun::get_barrel_smoke)
			.def("set_bullet_filter", &gun::set_bullet_filter)
			.def("drop_logic", &gun::drop_logic)
			.def_readwrite("current_rounds", &gun::current_rounds)
			.def_readwrite("trigger_mode", &gun::trigger_mode)
			.def_readwrite("current_state", &gun::current_state)
			.def_readwrite("target_camera_to_shake", &gun::target_camera_to_shake)
			.def_readwrite("bullets_once", &gun::bullets_once)
			.def_readwrite("max_rounds", &gun::max_rounds)
			.def_readwrite("spread_degrees", &gun::spread_degrees)
			.def_readwrite("bullet_damage", &gun::bullet_damage)
			.def_readwrite("bullet_speed", &gun::bullet_speed)
			.def_readwrite("shooting_interval_ms", &gun::shooting_interval_ms)
			.def_readwrite("max_bullet_distance", &gun::max_bullet_distance)
			.def_readwrite("melee_filter", &gun::melee_filter)
			.def_readwrite("melee_obstruction_filter", &gun::melee_obstruction_filter)
			.def_readwrite("bullet_distance_offset", &gun::bullet_distance_offset)
			.def_readwrite("shake_radius", &gun::shake_radius)
			.def_readwrite("shake_spread_degrees", &gun::shake_spread_degrees)
			.def_readwrite("is_automatic", &gun::is_automatic)
			.def_readwrite("bullet_render", &gun::bullet_render)
			.def_readwrite("swing_duration", &gun::swing_duration)
			.def_readwrite("swing_radius", &gun::swing_radius)
			.def_readwrite("swing_angle", &gun::swing_angle)
			.def_readwrite("swing_angular_offset", &gun::swing_angular_offset)
			.def_readwrite("swing_interval_ms", &gun::swing_interval_ms)
			//.def_readwrite("query_vertices", &gun::query_vertices)
			.def_readwrite("bullet_body", &gun::bullet_body)
			.enum_("constants")[
				luabind::value("NONE", gun::NONE),
				luabind::value("MELEE", gun::MELEE),
				luabind::value("SHOOT", gun::SHOOT),
				luabind::value("READY", gun::READY),
				luabind::value("SWINGING", gun::SWINGING),
				luabind::value("SWINGING_INTERVAL", gun::SWINGING_INTERVAL),
				luabind::value("SHOOTING_INTERVAL", gun::SHOOTING_INTERVAL)
			]
			;
	}
}