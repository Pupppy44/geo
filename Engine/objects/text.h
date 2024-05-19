#pragma once
#include "../tree/object.h"
#include <dwrite.h>

namespace geo {
	namespace objects {
		class text : public tree::object {
		public:
			text();

			void init();
			void render();
		private:
			IDWriteTextFormat* text_format = 0;
			IDWriteFactory* write_factory = NULL;
			IDWriteFontCollection* font_collection = NULL;
			ID2D1SolidColorBrush* brush = 0;
		};
	}
}
