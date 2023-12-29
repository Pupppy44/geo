#pragma once
#include "../tree/object.h"

namespace geo {
	namespace core {
		class game;
	}

	namespace network {
		class event : public tree::object {
		public:
			event(core::game&);

			void init();
			void render();
		private:
			core::game* game;
		};
	}
}