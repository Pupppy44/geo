#pragma once
#include "../tree/object.h"

namespace geo {
	namespace objects {
		class image : public tree::object {
		public:
			image();

			void init();
			void render();
		};
	}
}
