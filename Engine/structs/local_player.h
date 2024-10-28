#pragma once
#include <string>
#include <sol/sol.hpp>
#include "../util/helpers.h"

namespace geo {
	namespace structs {
		struct local_player {
			std::string username = util::get_username();
			std::string avatar = util::get_user_avatar();
			std::string id = util::get_user_id();

			static void bind(sol::state& lua) {
				lua.new_usertype<local_player>("local_player",
					"username", &local_player::username,
					"avatar", &local_player::avatar,
					"id", &local_player::id
				);
			}
		};
	}
}