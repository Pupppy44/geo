#pragma once
#include <string>
#include <sol/sol.hpp>

namespace geo {
	namespace tree {
		enum callback_type {
			UNKNOWN,
			// Events
			EVENT_ON_FIRE,
			// Input System
			INPUT_ON_CLICK,
			INPUT_ON_HOVER,
			INPUT_ON_KEY_DOWN,
			INPUT_ON_KEY_UP,
			// Players
			PLAYERS_ON_JOIN,
			PLAYERS_ON_LEAVE,
		};
		
		struct callback {
			std::string name;
			callback_type type = UNKNOWN;
			std::string id;
			sol::function function;
		};
	}
}