#pragma once
#include "../tree/object.h"

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
		private:
			void update_scrollbar();
		private:
			float scroll_position = 0;			
			D2D1_LAYER_PARAMETERS layer_params = {};
		};
	}
}
