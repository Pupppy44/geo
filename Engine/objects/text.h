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

			void create_text_resources();
		public:
			IDWriteTextLayout* text_layout = NULL; // Public for GetMetrics
		private:
			IDWriteTextFormat* text_format = NULL;
			IDWriteFactory* write_factory = NULL;
			IDWriteFontCollection* font_collection = NULL;
			ID2D1SolidColorBrush* brush = NULL;
		};
	}
}
