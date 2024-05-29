#pragma once
#include <string>
#include <sol/sol.hpp>

namespace geo {
	namespace structs {
		struct game_info {
			// Game information
			std::string name = "Untitled Game";
			std::string id = "-1";
#ifdef SERVER
			std::string type = "online";
#else
			std::string type = "local";
#endif
			std::string thumbnail;
			std::string background = "#000000";
			int width = 1920;
			int height = 1080;

			static void bind(sol::state& lua) {
				lua.new_usertype<game_info>("game_info",
					"name", &game_info::name,
					"id", &game_info::id,
					"type", &game_info::type,
					"thumbnail", &game_info::thumbnail,
					"background", &game_info::background,
					"width", &game_info::width,
					"height", &game_info::height
				);
			}
		};
	}
}