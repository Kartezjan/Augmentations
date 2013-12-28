#pragma once
#include "stdafx.h"
#include "bindings.h"

#include "../messages/destroy_message.h"

#include "entity_system/entity.h"

namespace bindings {
	luabind::scope _destroy_message() {
		return (
			luabind::class_<destroy_message>("destroy_message")
			.def(luabind::constructor<>())
			.def_readwrite("subject", &destroy_message::subject)
			.def_readwrite("redirection", &destroy_message::redirection)
			.def_readwrite("only_children", &destroy_message::only_children)
			);
	}
}