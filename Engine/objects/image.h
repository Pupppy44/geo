#pragma once
#include "../tree/object.h"
#include <wincodec.h>

namespace geo {
	namespace objects {
		class image : public tree::object {
		public:
			image();

			void init();
			void render();

			// Update the image when the path changes
			void update();
		private:
			std::string current_path = "";
			
			ID2D1Bitmap* bitmap = NULL;
			ID2D1Factory* factory = NULL;
			IWICImagingFactory* wic_factory = NULL;
			IWICBitmapDecoder* decoder = NULL;
			IWICBitmapFrameDecode* source = NULL;
			IWICFormatConverter* converter = NULL;
			ID2D1RoundedRectangleGeometry* border = NULL;
		};
	}
}
