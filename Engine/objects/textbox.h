#include "../tree/object.h"
#include <string>

namespace geo {
	// Scrollboxes, used for displaying child objects in a scrollable area
	namespace objects {
		class textbox : public tree::object {
		public:
			textbox();

			void init();
			void render();
			void message(UINT, WPARAM, LPARAM);
		private:
			D2D1_ROUNDED_RECT rectangle = D2D1::RoundedRect(D2D1::RectF(0, 0, 0, 0), 0, 0);
			ID2D1SolidColorBrush* brush = 0;
			ID2D1SolidColorBrush* outline_brush = 0;
		};
	}
}