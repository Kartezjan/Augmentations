#pragma once
#include <functional>

namespace augs {
	struct lua_state_wrapper;
}

void bind_game_framework_and_augs(augs::lua_state_wrapper&);