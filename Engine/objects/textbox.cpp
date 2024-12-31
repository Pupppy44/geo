#include "textbox.h"

namespace geo {
	namespace objects {
		textbox::textbox() {
			type("rectangle");
		};

		void textbox::init() {
			rect_object.context = context;
			rect_object.game = game;
			rect_object.init();

			text_object.context = context;
			text_object.game = game;
			text_object.init();
		};

		void textbox::render() {
			// Textbox properties
			std::string rendered_value = " " + value + " ";
			float x = get_property<float>("x");
			float y = get_property<float>("y");
			float width = get_property<float>("width");
			float height = get_property<float>("height");
			float radius = get_property<float>("radius");
			std::string color = get_property<std::string>("color");
			float opacity = get_property<float>("opacity");
			float outline_width = get_property<bool>("enable_outline", true) ? ((focused ? 5 : (hovered ? 4 : 3))) : 0;
			std::string outline_color = get_property<std::string>("outline_color");
			float font_size = height * get_property<float>("font_scale", 0.6f);
			float text_x = x - x_offset;
			float text_y = y + (height / 2) - (font_size / 2); // Center the text vertically
			std::string text_color = get_property<std::string>("text_color");
			std::string placeholder = get_property<std::string>("placeholder");
			
			// Handle placeholder text color
			if (value == "") {
				text_color = get_property<std::string>("placeholder_color", "#d2d2d4");
				rendered_value = " " + placeholder + " ";
			}

			// Apply corresponding properties to the rectangle object & render
			rect_object.set_properties(get_properties());
			rect_object.set_property({ tree::property_type::NUMBER, "outline_width", std::to_string(outline_width) });
			rect_object.render();

			// Apply corresponding properties to the text object
			text_object.set_properties(get_properties());
			text_object.set_property({ tree::property_type::NUMBER, "x", std::to_string(text_x) });
			text_object.set_property({ tree::property_type::NUMBER, "y", std::to_string(text_y) });
			text_object.set_property({ tree::property_type::NUMBER, "width", std::to_string(pow(9, 9)) });
			text_object.set_property({ tree::property_type::STRING, "text", rendered_value });
			text_object.set_property({ tree::property_type::STRING, "color", text_color });
			text_object.set_property({ tree::property_type::NUMBER, "size", std::to_string(font_size) });

			// Set up a clipping area to restrict text rendering within the textbox bounds
			D2D1_RECT_F clip_rect = D2D1::RectF(x, y, x + width, y + height);
			context->PushAxisAlignedClip(clip_rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			
			// Calculate text width
			text_object.create_text_resources();
			DWRITE_TEXT_METRICS text_metrics;
			text_object.text_layout->GetMetrics(&text_metrics);
			float text_width = text_metrics.width;

			// Adjust x_offset if text overflows
			if (text_width > width) {
				x_offset = text_width - width;
			}
			else {
				x_offset = 0;
			}

			text_object.render();
			
			render_caret(text_width, text_x);
			update_cursor();

			set_property({ tree::property_type::STRING, "value", value });

			context->PopAxisAlignedClip();
		};

		void textbox::message(UINT type, WPARAM wparam, LPARAM lparam) {
			POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

			switch (type) {
			case WM_CHAR:
				if (focused) {
					caret_visible = true;
					
					if (wparam == VK_BACK) {
						if (value.length() > 0) {
							value.pop_back();
						}
					}
					else {
						if (wparam < 32 || wparam > 126) {
							return;
						}

						value += (char)wparam;
					}
				}
				break;

			case WM_MOUSEMOVE:
				hovered = is_point_in_textbox(pt);
				break;
			case WM_LBUTTONDOWN:
				// Handle focus on click
				if (is_point_in_textbox(pt)) {
					focused = true;
				}
				else {
					focused = false;
				}
				break;
			case WM_KEYDOWN:
				if (!((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (wparam == 'V'))) {
					return;
				}
				__fallthrough;
			case WM_PASTE:
				if (focused && get_property<bool>("allow_paste", true)) {
					std::string clipboard_text = util::get_clipboard_text();
					for (char c : clipboard_text) {
						if (c >= 32 && c <= 126) {
							value += c;
						}
					}
				}	
				break;
			}

			for (auto& child : children) {
				child->message(type, wparam, lparam);
			}
		};

		void textbox::render_caret(float text_width, float text_x) {
			/*
			* Conditions to not render the caret:
			* 1. The textbox is not focused
			* 2. The caret is not enabled
			* 
			* If the value is empty, we'll render a caret at the beginning
			* of the textbox by adjusting the text_x and text_width values.
			*/
			if (!focused) return;
			if (!get_property<bool>("enable_caret", true)) return;
			if (value == "") {
				text_x = get_property<float>("x") + 10.0f;
				text_width = 0;
			}

			// Handle caret blinking
			auto current_time = std::chrono::steady_clock::now();
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_caret_time).count();

			if (elapsed_time >= 500) {
				last_caret_time = current_time;
				caret_visible = !caret_visible;
			}

			if (caret_visible) {
				float x = get_property<float>("x");
				float y = get_property<float>("y");
				float height = get_property<float>("height");

				float caret_x_offset = get_property<float>("caret_x_offset", -6.0f);
				float caret_x = text_x + text_width + caret_x_offset;

				std::string caret_color = get_property<std::string>("caret_color", "#FFFFFF");
				ID2D1SolidColorBrush* caret_brush;
				context->CreateSolidColorBrush(util::hex_to_color(caret_color), &caret_brush);
				
				if (caret_brush == nullptr) {
					return WARN("failed to create caret brush for textbox (id=" + get_property<std::string>("id") + ")");
				}

				float caret_padding = get_property<float>("caret_padding", 6.0f);
				D2D1_POINT_2F start = D2D1::Point2F(caret_x, y + caret_padding);
				D2D1_POINT_2F end = D2D1::Point2F(caret_x, y + height - caret_padding);

				float caret_width = get_property<float>("caret_width", 1.0f);
				
				context->DrawLine(start, end, caret_brush, caret_width);

				caret_brush->Release();
			}
		}

		void textbox::update_cursor() {
			if (hovered) {
				SetCursor(LoadCursor(NULL, IDC_IBEAM));
			}
			else {
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
		}

		bool textbox::is_point_in_textbox(POINT pt) {
			float x = get_property<float>("x");
			float y = get_property<float>("y");
			float width = get_property<float>("width");
			float height = get_property<float>("height");

			return (pt.x >= x && pt.x <= x + width && pt.y >= y && pt.y <= y + height);
		}
	}
}