#pragma once
#include "../tree/object.h"
#include "../util/rich_text.h"

namespace geo {
	namespace objects {
		class text : public tree::object {
		public:
			text();

			void init();
			void render();
		private:
			IDWriteTextFormat* text_format = NULL;
			IDWriteFactory* write_factory = NULL;
			IDWriteTextLayout* text_layout = NULL;
			IDWriteFontCollection* font_collection = NULL;
			ID2D1SolidColorBrush* brush = NULL;
		};
	}
}
