#pragma once
#include "../tree/object.h"
#include <windowsx.h>

/*
* Instead of using a render target, just render the children objects directly to the main render target and use the scrollbox object to set the clipping area
* * Add tree::object::children (std::vector<tree::object*>), can be used to store child objects for even other objects
* Add NATIVE event listeners for mouseclick down/up, mouse move, then we're good!
* Do all the logic after that
* Properties: x, y, width, height, border, radius, border_color (outline color)
*/

namespace geo {
	// Scrollboxes, used for displaying child objects in a scrollable area
	namespace objects {
		class scrollbox : public tree::object {
		public:
			scrollbox();

			void init();
			void render();
			void message(UINT, WPARAM, LPARAM);
		private:
			bool is_point_in_scrollbar(POINT);
			void update_scrollbar();
			void calculate_max_content_height();
		private:
			float max_content_height = 0.0f;
			float scroll_offset_y = 0.0f;
			bool is_dragging_scrollbar = false;
			bool is_hovering = false;
			float drag_start_y = 0.0f;

			D2D1_LAYER_PARAMETERS layer_params = {};
		};
	}
}
