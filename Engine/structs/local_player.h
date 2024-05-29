#pragma once
#include <string>
#include <sol/sol.hpp>
#include "../util/helpers.h"

namespace geo {
	namespace structs {
		struct local_player {
			std::string username = util::get_username();
			std::string avatar = "C:\\Users\\" + username + "\\AppData\\Local\\Temp\\" + username + ".bmp";

			static void bind(sol::state& lua) {
				lua.new_usertype<local_player>("local_player",
					"username", &local_player::username,
					"avatar", &local_player::avatar
				);
			}
		};
	}
}