#include "scrollbox.h"

namespace geo {
	namespace objects {
		scrollbox::scrollbox() {
			type("scrollbox");
		};

		void scrollbox::init() {
			// Create a layer for the clipping area
			layer_params.geometricMask = nullptr;
			layer_params.maskTransform = D2D1::Matrix3x2F::Identity();
			layer_params.opacityBrush = nullptr;
			layer_params.opacity = 1.0f;

			// Initialize the children vector
			for (auto child : children) {
				child->context = context;
				child->init();
			}
		};

		void scrollbox::render() {
			float x = get_property<float>("x");
			float y = get_property<float>("y");
			float width = get_property<float>("width");
			float height = get_property<float>("height");
			
			// Define the clipping rectangle and positioning
			layer_params.contentBounds = D2D1::RectF(0, 0, width, height);
			context->SetTransform(D2D1::Matrix3x2F::Translation(x, y));

			// Push the layer with the clipping rectangle
			context->PushLayer(layer_params, nullptr);

			// Render the children objects
			for (auto child : children) {
				child->render();
			}

			update_scrollbar();

			// Pop the layer to restore the previous state
			context->PopLayer();
			context->SetTransform(D2D1::Matrix3x2F::Identity());
		}
		
		void scrollbox::update_scrollbar() {
			float height = get_property<float>("height");
			
			D2D1_ROUNDED_RECT bar = D2D1::RoundedRect(D2D1::RectF(0, 0, 7, height), 5, 5);
			
			ID2D1SolidColorBrush* brush;
			context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);

			context->DrawRoundedRectangle(bar, brush);
			context->FillRoundedRectangle(bar, brush);

			brush->Release();
		}
	}
}