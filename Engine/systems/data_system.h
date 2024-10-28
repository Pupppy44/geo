#pragma once
#include "../tree/object.h"

namespace geo {
	namespace core {
		class game;
	}

	namespace systems {
		// System for saving game data to the disk
		class data_system : public tree::object {
		public:
			data_system(core::game&);

			void init() {};
			void render() {};
		private:
			core::game* game;
			std::string data_path = std::string(std::getenv("APPDATA")) + "/Geo/";
		};
	}
}

