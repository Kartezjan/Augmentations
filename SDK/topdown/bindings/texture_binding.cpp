#pragma once
#include "stdafx.h"
#include "bindings.h"

#include "texture_baker/texture_baker.h"
#include "../game/texture_helper.h"

namespace bindings {
	luabind::scope _texture() {
		return (
			luabind::class_<texture_baker::texture>("raw_texture"),

			luabind::class_<texture_helper>("texture")
			.def(luabind::constructor<std::wstring, texture_baker::atlas&>())
			.def_readwrite("tex", &texture_helper::tex),

			luabind::class_<texture_baker::atlas>("atlas")
			.def(luabind::constructor<>())
			.def("build", &texture_baker::atlas::default_build)
			);
	}
}