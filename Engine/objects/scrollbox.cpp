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

			float x = get_property<float>("x");
			float y = get_property<float>("y");

			// Initialize the children vector
			for (auto child : children) {
				child->context = context;
				child->init();
			}

			// Calculate the total content height based on the original Y values of children
			calculate_max_content_height();
		};

		void scrollbox::calculate_max_content_height() {
			// Calculate the total height of the content by summing the original Y positions
			max_content_height = 0;
			for (auto child : children) {
				float child_y = child->get_property<float>("y");
				float child_height = child->get_property<float>("height");
				max_content_height = max(max_content_height, child_y + child_height); // Sum the height of each child
			}
		}

		void scrollbox::render() {
			float x = get_property<float>("x");
			float y = get_property<float>("y");
			float width = get_property<float>("width");
			float height = get_property<float>("height");

			// Define the clipping rectangle with vertical offset for scrolling
			layer_params.contentBounds = D2D1::RectF(x, y + scroll_offset_y, x + width, y + height + scroll_offset_y);

			// Push the layer with the clipping rectangle
			context->PushLayer(layer_params, nullptr);

			// Render the children objects
			for (auto child : children) {
				float child_x = child->get_property<float>("x");
				float child_y = child->get_property<float>("y") - scroll_offset_y; // Adjust based on the scroll offset
				float original_x = child_x;
				float original_y = child_y;

				// Update the child's position to account for the scrollbox's Y position
				child->set_property({ tree::property_type::NUMBER, "x", std::to_string(child_x) });
				child->set_property({ tree::property_type::NUMBER, "y", std::to_string(child_y) });

				child->render();

				// Reset the child's position to the original values
				child->set_property({ tree::property_type::NUMBER, "x", std::to_string(original_x) });
				child->set_property({ tree::property_type::NUMBER, "y", std::to_string(original_y) });
			}

			// Update the scrollbar position to reflect the Y scrolling
			update_scrollbar();

			// Pop the layer to restore the previous state
			context->PopLayer();
			context->SetTransform(D2D1::Matrix3x2F::Identity());
		}

		void scrollbox::message(UINT msg, WPARAM wParam, LPARAM lParam) {
			// Handle the scrolling logic here (only vertical scroll - up and down)
			switch (msg) {
			case WM_MOUSEWHEEL: {
				// Handle the mouse wheel scrolling to adjust scroll_offset_y (vertical scroll)
				int wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);
				scroll_offset_y += (wheel_delta > 0) ? -10.0f : 10.0f;  // Scroll up or down
				scroll_offset_y = max(scroll_offset_y, 0.0f);  // Prevent scrolling above the top of the content

				// Prevent scrolling past the bottom of the content
				float max_scroll_y = max_content_height - get_property<float>("height");
				scroll_offset_y = min(scroll_offset_y, max_scroll_y); // Prevent scrolling past the content
				break;
			}

			case WM_LBUTTONDOWN: {
				// Check if the mouse click is on the scrollbar and initiate dragging
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

				// If the click is within the scrollbar (define your scrollbar's bounds)
				if (is_point_in_scrollbar(pt)) {
					// Start dragging the scrollbar (set a flag or capture the mouse)
					is_dragging_scrollbar = true;
					drag_start_y = pt.y;  // Save the starting Y position of the drag
				}
				break;
			}

			case WM_MOUSEMOVE: {
				// If dragging is active, adjust the scroll_offset_y based on mouse movement
				if (is_dragging_scrollbar) {
					POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					int delta_y = pt.y - drag_start_y;  // Calculate the vertical movement distance
					scroll_offset_y += delta_y;  // Adjust vertical scroll position
					scroll_offset_y = max(scroll_offset_y, 0.0f);  // Prevent scrolling above top

					// Prevent scrolling past the bottom of the content
					float max_scroll_y = max_content_height - get_property<float>("height");
					scroll_offset_y = min(scroll_offset_y, max_scroll_y); // Prevent scrolling past the bottom

					drag_start_y = pt.y;  // Update the drag start position for smooth scrolling
				}
				break;
			}

			case WM_LBUTTONUP: {
				// End dragging when the mouse button is released
				if (is_dragging_scrollbar) {
					is_dragging_scrollbar = false;
					ReleaseCapture();  // Release the mouse capture
				}
				break;
			}

			default:
				break;
			}

			// Pass the message to the children (if needed)
			for (auto child : children) {
				child->message(msg, wParam, lParam);
			}
		}

		void scrollbox::update_scrollbar() {
			float x = get_property<float>("x") + 3;
			float y = get_property<float>("y") + 3;
			float height = get_property<float>("height") / 2;

			if (children.size() == 0 || max_content_height == 0) {
				return;
			}

			// Adjust the height of the scrollbar based on the ratio of visible content to total content
			float scrollbar_height = max(height * (height / max_content_height), 20.0f); // Ensure a minimum size

			// Define the vertical scrollbar
			D2D1_ROUNDED_RECT bar = D2D1::RoundedRect(D2D1::RectF(x, y + scroll_offset_y, x + 7, y + scrollbar_height + scroll_offset_y), 5, 5);

			ID2D1SolidColorBrush* brush;
			context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 0.5f), &brush);

			// Draw and fill the scrollbar
			context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			context->DrawRoundedRectangle(bar, brush, 0);
			context->FillRoundedRectangle(bar, brush);

			brush->Release();
		}

		bool scrollbox::is_point_in_scrollbar(POINT pt) {
			// Define the bounds of the scrollbar (adjust as needed)
			float x = get_property<float>("x") + 3;
			float y = get_property<float>("y") + 3;
			float height = get_property<float>("height") / 2;
			RECT scrollbar_rect = { (int)x, (int)y, (int)(x + 7), (int)(y + height) };

			return (pt.x >= scrollbar_rect.left && pt.x <= scrollbar_rect.right &&
				pt.y >= scrollbar_rect.top && pt.y <= scrollbar_rect.bottom);
		}
	}
}
