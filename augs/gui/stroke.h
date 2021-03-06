#pragma once
#include "gui_world.h"

namespace augs {
	namespace gui {
		struct rect;
		struct solid_stroke {
			struct border {
				int width;
				material mat;
			} left, top, right, bottom;

			enum type {
				INSIDE, OUTSIDE
			} _type;

			solid_stroke(int width = 1, const material& = material(), type = OUTSIDE);

			void set_width(int);
			void set_material(const material&);

			void draw(std::vector<augs::vertex_triangle>& out, rects::ltrb<float> origin, const rects::ltrb<float>* clipper = nullptr) const;
			void draw(std::vector<augs::vertex_triangle>& out, const rect&) const;
		};
	}
}