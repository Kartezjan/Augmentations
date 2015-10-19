#pragma once
#include "stdafx.h"
#include "bindings.h"

#include "../messages/particle_burst_message.h"
#include "entity_system/entity.h"

namespace bindings {
	luabind::scope _particle_burst_message() {
		return
			luabind::class_<particle_burst_message, message>("particle_burst_message")
			.def(luabind::constructor<>())
			.def_readwrite("pos", &particle_burst_message::pos)
			.def_readwrite("type", &particle_burst_message::type)
			.def_readwrite("rotation", &particle_burst_message::rotation)
			.def("set_effect", &particle_burst_message::set_effect)
			.def_readwrite("local_transform", &particle_burst_message::local_transform)
			.def_readwrite("target_group_to_refresh", &particle_burst_message::target_group_to_refresh)
			.enum_("burst_type")[
				luabind::value("BULLET_IMPACT", particle_burst_message::burst_type::BULLET_IMPACT),
				luabind::value("CUSTOM", particle_burst_message::burst_type::CUSTOM),
				luabind::value("WEAPON_SHOT", particle_burst_message::burst_type::WEAPON_SHOT)
			];
	}
}