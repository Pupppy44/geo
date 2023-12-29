#pragma once
#include <string>
#include <sol/sol.hpp>

namespace geo {
	namespace tree {
		enum callback_type {
			UNKNOWN,
			EVENT_ON_FIRE,
			INPUT_ON_KEY_DOWN,
			INPUT_ON_KEY_UP
		};
		
		struct callback {
			std::string name;
			callback_type type = UNKNOWN;
			std::string id;
			sol::function function;
		};
	}
}