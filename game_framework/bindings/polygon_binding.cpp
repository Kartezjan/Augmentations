#pragma once
#include "stdafx.h"
#include <luabind/iterator_policy.hpp>
#include "bindings.h"

#include "../shared/drawing_state.h"

#include "../components/polygon_component.h"

struct dummy_uv_mapping_mode {

};

namespace bindings {
	luabind::scope _polygon() {
		return 	
			(

			luabind::class_<dummy_uv_mapping_mode>("uv_mapping_mode")
			.enum_("position_copying_type")[
				luabind::value("STRETCH", polygon::uv_mapping_mode::STRETCH),
					luabind::value("OVERLAY", polygon::uv_mapping_mode::OVERLAY)
			],

			luabind::class_<vertex>("vertex")
			.def(luabind::constructor<vec2>())
			.def(luabind::constructor<vec2, vec2, pixel_32, texture&>())
			.def("set_texcoord", &vertex::set_texcoord)
			.def_readwrite("pos", &vertex::pos)
			.def_readwrite("texcoord", &vertex::texcoord)
			.def_readwrite("color", &vertex::color)
			,

			luabind::class_<vertex_triangle>("vertex_triangle")
			.def(luabind::constructor<>())
			.def("get_vert", &vertex_triangle::get_vert)
			,

			luabind::class_<polygon::concave>("drawable_concave")
			.def(luabind::constructor<>())
			.def("add_vertex", &polygon::concave::add_vertex)
			,

			luabind::class_<polygon>("polygon")
			.def(luabind::constructor<>())
			.def("add_concave", &polygon::add_concave)
			.def("get_vertex_count", &polygon::get_vertex_count)
			.def("get_vertex", &polygon::get_vertex)
			.def("draw", &polygon::draw)
			);
	}
}