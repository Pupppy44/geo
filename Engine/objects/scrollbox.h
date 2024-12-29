#pragma once
#include "../tree/object.h"
#include <windowsx.h>

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

			void render_border();
			void update_border();

			void calculate_max_content_height();
		private:
			float max_content_height = 0.0f;
			float scroll_offset_y = 0.0f;  
			bool is_dragging_scrollbar = false; 
			bool is_hovering = false;
			float drag_start_y = 0.0f;
			
			ID2D1Factory* factory = NULL;
			ID2D1RoundedRectangleGeometry* border = NULL;
			D2D1_LAYER_PARAMETERS layer_params = {};
		};
	}
}