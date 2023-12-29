#pragma once
#include <string>
#include <functional>
#include <sol/sol.hpp>

namespace geo {
	namespace tree {
		struct function {
			std::string name;
			std::function<sol::object(sol::variadic_args)> function;
		};
	}
}