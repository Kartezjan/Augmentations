#pragma once
#include "../rect.h"
#include <functional>
#include "../text/text_rect.h"

namespace augs {
	namespace gui {
		namespace controls {
			class button : public rect {
			public:
				std::function<void()> on_click;
				std::function<void()> on_hover;
				std::function<void()> on_lmousedown;
				std::function<void()> on_lmouseup;

				button(const rect& = rect(),
					const std::function<void()>& on_click = nullptr,
					const std::function<void()>& on_hover = nullptr,
					const std::function<void()>& on_lmousedown = nullptr,
					const std::function<void()>& on_lmouseup = nullptr);

				void consume_gui_event(event_info m) override;
			};

			struct text_button : public button {
				text::text_rect label;

				/* centers label */
				text_button(const button&, const text::fstr&);
				text_button(const button&, vec2i, const text::fstr&);

				void get_member_children(std::vector<rect_id>& children) override;

				void draw_label(draw_info);
				void center();
			};
		}
	}
}