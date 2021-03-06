#include "overworld.h"

namespace augs {
	overworld::overworld() : delta_timer(60.0, 5) {

	}

	void overworld::configure_stepping(float fps, int max_updates_per_step) {
		delta_timer = augs::fixed_delta_timer(fps, max_updates_per_step);
	}
	
	double overworld::delta_seconds() const {
		return delta_ms / 1000.0;
	}

	double overworld::delta_milliseconds() const {
		return delta_ms;
	}

	double overworld::view_interpolation_ratio() const {
		return delta_timer.fraction_of_time_until_the_next_logic_step();
	}

	void overworld::assign_frame_time_to_delta_for_drawing_time_systems() {
		delta_ms = frame_timer.extract<std::chrono::milliseconds>();
	}

	void overworld::restore_fixed_delta() {
		delta_ms = delta_timer.delta_milliseconds();
	}
}