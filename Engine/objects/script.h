#pragma once
#include "../tree/object.h"
using namespace geo::tree;

namespace geo {
	namespace objects {
		class script : public object {
		public:
			script();

			void init();
			void render();
		};
	}
}
