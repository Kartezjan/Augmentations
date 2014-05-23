#pragma once
#include "stdafx.h"
#include "game_framework.h"

#include <gtest\gtest.h>
#include "augmentations.h"

#include "resources/scriptable_info.h"
#include "world_instance.h"

namespace framework {
	void init() {
		augs::init();

		int argc = 0;
		::testing::InitGoogleTest(&argc, (wchar_t**)nullptr);

		::testing::FLAGS_gtest_catch_exceptions = false;
		::testing::FLAGS_gtest_break_on_failure = false;
		auto result = RUN_ALL_TESTS();

		resources::script::script_reloader.report_errors = &std::cout;
	}

	void deinit() {
		augs::deinit();
	}

	void set_current_window(augs::window::glwindow& gl) {
		world_instance::global_window = &gl;
		gl.set_show(gl.SHOW);
		gl.current();
	}

}