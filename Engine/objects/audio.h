#pragma once
#include "../tree/object.h"
using namespace geo::tree;

namespace geo {
	namespace objects {
		class audio : public object {
		public:
			audio();
			
			void init();
			void render();
		};
	}
}

