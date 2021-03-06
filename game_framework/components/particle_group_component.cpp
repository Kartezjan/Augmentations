#include "particle_group_component.h"
#include "../shared/state_for_drawing.h"

using namespace augs;

namespace components {
	void particle_group::draw(shared::state_for_drawing_renderable in) {
		for (auto& s : stream_slots)
			for (auto& it : s.particles.particles) {
				auto temp_alpha = it.face.color.a;

				if (it.should_disappear) {
					auto desired_alpha = static_cast<rgba_channel>(((it.max_lifetime_ms - it.lifetime_ms) / it.max_lifetime_ms) * static_cast<float>(temp_alpha));
					if (it.alpha_levels > 0) {
						it.face.color.a = desired_alpha == 0 ? 0 : ((255 / it.alpha_levels) * (1 + (desired_alpha / (255 / it.alpha_levels))));
					}
					else {
						it.face.color.a = desired_alpha;
					}
				}

				in.renderable_transform = it.ignore_rotation ? components::transform(it.pos, 0) : components::transform({ it.pos, it.rotation });
				it.face.draw(in);
				it.face.color.a = temp_alpha;
			}
	}

}