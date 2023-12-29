#pragma once
#include "../tree/object.h"

namespace geo {
	namespace core {
		class game;
	}
	
	namespace systems {
		// System for handling keyboard and mouse input
		class input_system : public tree::object {
		public:
			input_system(core::game&);

			void init() {};
			void render() {};
		private:
			core::game* game;
		};
	}
}

