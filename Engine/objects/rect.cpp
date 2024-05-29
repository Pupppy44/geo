#include "rect.h"

namespace geo {
	namespace objects {
		rect::rect() {
			type("rectangle");
		};

		void rect::init() {};

		void rect::render() {
			// Rectangle properties
			float x = get_property<float>("x");
			float y = get_property<float>("y");
			float width = get_property<float>("width");
			float height = get_property<float>("height");
			float radius = get_property<float>("radius");
			std::string color = get_property<std::string>("color");

			// Outline properties
			float outline_width = get_property<float>("outline_width");
			std::string outline_color = get_property<std::string>("outline_color");

			// Set the antialias mode
			if (abs(radius) > 0) {
				context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			}
			else {
				context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
			}

			// Update the rectangle object
			rectangle = D2D1::RoundedRect(
				D2D1::RectF(x, y, width + x, height + y),
				radius,
				radius
			);

			// Update the brush
			context->CreateSolidColorBrush(
				util::hex_to_color(color),
				&brush
			);

			if (brush == 0) {
				return;
			}

			// Set opacity for fill brush
			brush->SetOpacity(get_property<float>("opacity"));

			// Update the outline brush			
			context->CreateSolidColorBrush(
				util::hex_to_color(outline_color),
				&outline_brush
			);

			if (outline_brush == 0) {
				return;
			}

			// Rotation
			context->SetTransform(
				D2D1::Matrix3x2F::Rotation(
					get_property<float>("rotation"),
					D2D1::Point2F(x + width / 2, y + height / 2)
				)
			);
			
			// Draw the rectangle onto the screen
			context->DrawRoundedRectangle(
				rectangle,
				outline_brush,
				outline_width,
				NULL
			);
			
			// Fill the rectangle
			context->FillRoundedRectangle(
				rectangle,
				brush
			);

			// Reset the rotation
			context->SetTransform(D2D1::Matrix3x2F::Identity());
			
			// Release the brushes
			brush->Release();
			outline_brush->Release();	
		};
	}
}