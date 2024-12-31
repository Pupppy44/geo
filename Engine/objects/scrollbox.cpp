#include "scrollbox.h"

namespace geo {
	namespace objects {
		scrollbox::scrollbox() {
			type("scrollbox");
		};

		void scrollbox::init() {
			context->GetFactory(&factory); // For layer geometry

			layer_params.geometricMask = nullptr;
			layer_params.maskTransform = D2D1::Matrix3x2F::Identity();
			layer_params.opacityBrush = nullptr;
			layer_params.opacity = 1.0f;

			// Initialize children
			for (auto child : children) {
				child->context = context;
				child->game = game;
				child->init();
			}

			// Calculate the max height of the content
			calculate_max_content_height();
		};

		void scrollbox::calculate_max_content_height() {
			max_content_height = 0.0f;
			// Iterate through children to get the max height
			for (auto child : children) {
				float child_y = child->get_property<float>("y");
				float child_height = child->get_property<float>("height");
				if (child_y + child_height > max_content_height) {
					max_content_height = child_y + child_height;
				}
			}
		}

        void scrollbox::render() {
            float x = get_property<float>("x");
            float y = get_property<float>("y");
            float width = get_property<float>("width");
            float height = get_property<float>("height");

			// Create a layer for the scrollbox (aka. the border)
			update_border();
			context->PushLayer(
				D2D1::LayerParameters(
					D2D1::RectF(x, y, x + width, y + height), // Content bounds
					border
				),
				nullptr
			);

            // Render the children based on the scroll offset
            for (auto child : children) {
                float original_x = child->get_property<float>("x");
                float child_x = child->get_property<float>("x") + x;
				float original_y = child->get_property<float>("y");
				float child_y = child->get_property<float>("y") - scroll_offset_y + y;

				// Offset based on the scroll offset and parent position
                child->set_property({ tree::property_type::NUMBER, "x", std::to_string(child_x) });
				child->set_property({ tree::property_type::NUMBER, "y", std::to_string(child_y)});

                // Render the child
                child->render(); 

                child->set_property({ tree::property_type::NUMBER, "x", std::to_string(original_x) });
                child->set_property({ tree::property_type::NUMBER, "y", std::to_string(original_y) });
            }

			// Update the scrollbar & border
            update_scrollbar();
			render_border();

            context->PopLayer();

			border->Release();
        }

        void scrollbox::message(UINT msg, WPARAM wParam, LPARAM lParam) {
            switch (msg) {
            case WM_MOUSEWHEEL: {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);

				// Adjusts scroll speed based on scrollbox height (indirect relationship)
                float scroll_ratio = max_content_height / get_property<float>("height");
                scroll_offset_y -= delta / 120.0f * 10.0f * scroll_ratio;

                // Prevent overscrolling/underscrolling
                if (scroll_offset_y > max_content_height - get_property<float>("height") - 6) {
                    scroll_offset_y = max_content_height - get_property<float>("height") - 6;
                }
                else if (scroll_offset_y < 0) {
                    scroll_offset_y = 0;
                }
                break;
            }
            case WM_LBUTTONDOWN: {
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                if (is_point_in_scrollbar(pt)) {
                    is_dragging_scrollbar = true;
                    drag_start_y = pt.y;
                }
                break;
            }
            case WM_MOUSEMOVE: {
                // Highlight on hover
                is_hovering = is_point_in_scrollbar({ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });

                if (is_dragging_scrollbar) {
                    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    int drag_distance = pt.y - drag_start_y;
                    // Adjusts scroll speed based on scrollbox height (indirect relationship)
					float scroll_ratio = get_property<float>("height") / max_content_height;
                    scroll_offset_y += drag_distance / scroll_ratio;

					// Prevent overscrolling/underscrolling
					if (scroll_offset_y > max_content_height - get_property<float>("height") - 6) {
                        scroll_offset_y = max_content_height - get_property<float>("height") - 6;
                    }
                    else if (scroll_offset_y < 0) {
                        scroll_offset_y = 0;
                    }

                    drag_start_y = pt.y; 
                }
                break;
            }

            case WM_LBUTTONUP: {
                is_dragging_scrollbar = false;
                break;
            }
            default:
                break;
            }

            // Send message to children if needed
            for (auto child : children) {
                child->message(msg, wParam, lParam);
            }
        }

		void scrollbox::update_scrollbar() {
			float x = get_property<float>("x") + 3;
			float y = get_property<float>("y") + 3;
			float height = get_property<float>("height");

			if (max_content_height <= height) {
				return;
			}

			// Calculate the scrollbar height based on content height vs visible height
			float scrollbar_height = (get_property<float>("height") / max_content_height) * get_property<float>("height");
			if (scrollbar_height < 20) {
				scrollbar_height = 20;
			}

			// Bar rect based on scroll_offset_y and max_content_height (more content, smaller bar)
            D2D1_ROUNDED_RECT bar = D2D1::RoundedRect(D2D1::RectF(x, y + (scroll_offset_y / max_content_height) * height, x + 7, y + (scroll_offset_y / max_content_height) * height + scrollbar_height), 5, 5);

			ID2D1SolidColorBrush* brush;
			context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 0.5f), &brush);

			if (brush == nullptr) {
                return WARN("failed to create brush for scrollbar (id=" + id() + ")");
			}

			if (is_dragging_scrollbar || is_hovering) {
				brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 0.8f));
			}

			context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			context->DrawRoundedRectangle(bar, brush, 0);
			context->FillRoundedRectangle(bar, brush);

			brush->Release();
		}

		void scrollbox::render_border() {
			if (border == nullptr) {
				update_border();
				render_border();
			}

			// Border Properties
			float x = get_property<float>("x");
			float y = get_property<float>("y");
			float width = get_property<float>("width");
			float height = get_property<float>("height");
			float border_radius = get_property<float>("border_radius");
			std::string border_color = get_property<std::string>("border_color");
			float border_width = get_property<float>("border_width");

			// Create the border brush
			ID2D1SolidColorBrush* brush;
			context->CreateSolidColorBrush(util::hex_to_color(border_color), &brush);

			if (brush == nullptr) {
				return WARN("failed to create brush for scrollbox border (id=" + id() + ")");
			}

			// Draw the border
			context->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), border_radius, border_radius), brush, border_width);

			brush->Release();
		}

        void scrollbox::update_border() {
			// Border Properties
            float x = get_property<float>("x");
            float y = get_property<float>("y");
			float width = get_property<float>("width");
            float height = get_property<float>("height");
			float border_radius = get_property<float>("border_radius");

			// Create the border geometry
			factory->CreateRoundedRectangleGeometry(
				D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), border_radius, border_radius),
				&border
			);
        }

        bool scrollbox::is_point_in_scrollbar(POINT pt) {
            float x = get_property<float>("x") + 3;
            float y = get_property<float>("y") + 3;
            float height = get_property<float>("height");
            float scrollbar_height = max((height / max_content_height) * height, 20.0f);
            float scrollbar_y = y + (scroll_offset_y / max_content_height) * height;

            return pt.x >= x && pt.x <= x + 7 && pt.y >= scrollbar_y && pt.y <= scrollbar_y + scrollbar_height;
        }
	}
}