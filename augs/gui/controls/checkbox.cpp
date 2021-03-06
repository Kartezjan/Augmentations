#pragma once
#include "checkbox.h"

namespace augs {
	namespace gui {
		namespace controls {
			checkbox::checkbox(const rect& r, bool set,
				const std::function<void(bool)>& callback) : rect(r), set(set), callback(callback) {
			}

			bool checkbox::get_state() const {
				return set;
			}

			void checkbox::set_state(bool flag) {
				if (flag != get_state()) {
					set = flag;
					on_change(set);
				}
			}

			void checkbox::on_change(bool) {
			}

			checkbox::operator bool() const {
				return get_state();
			}

			void checkbox::consume_gui_event(event_info e) {
				if (e == rect::gui_event::lclick ||
					e == rect::gui_event::keydown && (e.owner.state.key == augs::window::event::keys::ENTER)) {
					set_state(!get_state());
					if (callback) callback(get_state());
				}
				try_to_make_this_rect_focused(e);
				focus_next_rect_by_tab(e);
				focus_next_rect_by_arrows(e);
			}

			checklabel::checklabel(const checkbox& r, const std::wstring& label, const text::style& style_active, const text::style& style_inactive)
				: checkbox(r), active_text(rects::xywh<float>(), text::format(label, style_active)), inactive_text(rects::xywh<float>(), text::format(label, style_inactive)) {
				stretch_rc();
			}

			checklabel::checklabel(const checkbox& r, const text::fstr& active_str, const text::fstr& inactive_str)
				: checkbox(r), active_text(rects::xywh<float>(), active_str), inactive_text(rects::xywh<float>(), inactive_str) {
				stretch_rc();
			}

			void checklabel::get_member_children(std::vector<rect_id>& c) {
				c.push_back(&active_label());
			}

			void checklabel::on_change(bool set) {
				stretch_rc();
			}

			void checklabel::stretch_rc() {
				active_label().draft.guarded_redraw();
				auto size = active_label().draft.get_draft().get_bbox();
				rc.w(size.w);
				rc.h(size.h);
			}

			text::text_rect& checklabel::active_label() {
				return get_state() ? active_text : inactive_text;
			}
		}
	}
}