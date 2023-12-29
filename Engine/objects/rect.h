#pragma once
#include "../tree/object.h"

namespace geo {
	namespace objects {
		class rect : public tree::object {
		public:
			rect();

			void init();
			void render();
		private:
			D2D1_ROUNDED_RECT rectangle = D2D1::RoundedRect(D2D1::RectF(0, 0, 0, 0), 0, 0);
			ID2D1SolidColorBrush* brush = 0;
			ID2D1SolidColorBrush* outline_brush = 0;
		};
	}
}
