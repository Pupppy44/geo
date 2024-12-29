#pragma once
#include "../tree/object.h"

namespace geo {
	namespace core {
		class game;
	}

	namespace systems {
		// System for text-to-speech
		class animation_system : public tree::object {
		public:
			struct animation {
				std::string type;
				std::vector<std::pair<float, float>> path; // Can be empty if the animation doesn't move
				std::shared_ptr<tree::object> object;
			};

			struct path {
				std::vector<std::pair<float, float>> points;
				std::shared_ptr<tree::object> object;
			};

			animation_system(core::game&);

			void init() {};
			void render();
		private:
			// Rendering Functions
			void render_paths();
			void render_animations();

			// Path Functions
			std::vector<std::pair<float, float>> create_path(std::shared_ptr<tree::object>, std::string);
			sol::table path_to_table(lua_State*, std::vector<std::pair<float, float>>);
		private:
			core::game* game;
			// System data
			std::vector<path> paths;
			std::vector<animation> animations;

		};
	}
}
