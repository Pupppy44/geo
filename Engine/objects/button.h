#pragma once
#include "../tree/object.h"

namespace geo {
	namespace objects {
		class button : public tree::object {
		public:
			button();

			void init();
			void render();
		};
	}
}
