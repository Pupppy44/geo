#include "text.h"
#include "rect.h"
#include <string>
#include <chrono>
#include <windowsx.h>

namespace geo {
	// Textboxes for user input
	namespace objects {
		class textbox : public tree::object {
		public:
			textbox();

			void init();
			void render();
			void message(UINT, WPARAM, LPARAM);
		private:
			void render_caret(float, float);
			bool is_point_in_textbox(POINT);
		private:
			bool focused = false;
			bool hovered = false;
			std::string value = "";
			float x_offset = 0.0f;
			bool caret_visible = true;
			std::chrono::time_point<std::chrono::steady_clock> last_caret_time = std::chrono::steady_clock::now();
			objects::text text_object;
			objects::rect rect_object;
		};
	}
}