#include "rect.h"

namespace geo {
	namespace objects {
		rect::rect() {
			type("rectangle");
		};

		void rect::init() {
			context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		};

		void rect::render() {
			// Rectangle properties
			float x = get_property<float>("x");
			float y = get_property<float>("y");
			float width = get_property<float>("width");
			float height = get_property<float>("height");
			float radius = get_property<float>("radius");
			
			// Update the rectangle object
			rectangle = D2D1::RoundedRect(
				D2D1::RectF(x, y, width + x, height + y),
				radius,
				radius
			);

			// Update the brush
			context->CreateSolidColorBrush(
				D2D1::ColorF(255, 0, 0, 1),
				&brush
			);

			if (brush == 0) {
				return;
			}

			// Draw the rectangle onto the screen
			context->DrawRoundedRectangle(
				rectangle,
				brush,
				1,
				NULL
			);

			// Fill the rectangle
			context->FillRoundedRectangle(
				rectangle,
				brush
			);
		};
	}
}