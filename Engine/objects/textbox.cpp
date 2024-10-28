#include "text.h"
#include <string>

namespace geo {
	namespace objects {
		class textbox : public text {
		public:
			textbox() {
				type("textbox");
				input_text = ""; // Initialize with empty text
				is_focused = false;
			}

			void init() {
				text::init();
			}

			void render() override {
				// Retrieve properties
				auto x = get_property<float>("x");
				auto y = get_property<float>("y");
				auto w = get_property<float>("width");
				auto h = get_property<float>("height");
				auto align = get_property<std::string>("align");
				auto bg_color = util::hex_to_color(get_property<std::string>("bg_color"));
				auto border_color = util::hex_to_color(get_property<std::string>("border_color"));

				// Draw background
				ID2D1SolidColorBrush* bg_brush;
				context->CreateSolidColorBrush(bg_color, &bg_brush);
				context->FillRectangle(D2D1::RectF(x, y, x + w, y + h), bg_brush);
				bg_brush->Release();

				// Draw border
				ID2D1SolidColorBrush* border_brush;
				context->CreateSolidColorBrush(border_color, &border_brush);
				context->DrawRectangle(D2D1::RectF(x, y, x + w, y + h), border_brush, 1.0f);
				border_brush->Release();

				// Update text properties for rendering
				set_property(tree::property(tree::property_type::STRING, "text", input_text));
				set_property(tree::property(tree::property_type::NUMBER, "x", std::to_string(x)));
				set_property(tree::property(tree::property_type::NUMBER, "y", std::to_string(y)));
				set_property(tree::property(tree::property_type::NUMBER, "width", std::to_string(w)));
				set_property(tree::property(tree::property_type::NUMBER, "height", std::to_string(h)));
				set_property(tree::property(tree::property_type::STRING, "align", align));

				// Render the text using the inherited render method
				text::render();
			}

			void handle_input(WPARAM wParam) {
				if (is_focused) {
					if (wParam == VK_BACK && !input_text.empty()) {
						input_text.pop_back(); // Handle backspace
					}
					else if (wParam >= 32 && wParam <= 126) { // ASCII range for printable characters
						input_text += static_cast<char>(wParam); // Append character
					}
				}
			}

			void set_focus(bool focus) {
				is_focused = focus;
			}

		private:
			std::string input_text; // The text input by the user
			bool is_focused;         // Focus state for textbox
		};
	}
}
