#include "input_system.h"
#include "../core/game.h"

namespace geo {
	namespace systems {
		input_system::input_system(core::game& g) : game(&g) {
			type("input_system");

			define("on_key_down", [&](sol::variadic_args args) -> sol::object {
				// Create a key down callback
				tree::callback callback;
				callback.type = tree::callback_type::INPUT_ON_KEY_DOWN;
				callback.function = args[0].as<sol::function>();

				// Add to callbacks list
				game->runner.callbacks.inputs.push_back(callback);

				return sol::nil;
			});

			define("on_key_up", [&](sol::variadic_args args) -> sol::object {
				// Create a key up callback
				tree::callback callback;
				callback.type = tree::callback_type::INPUT_ON_KEY_UP;
				callback.function = args[0].as<sol::function>();

				// Add to callbacks list
				game->runner.callbacks.inputs.push_back(callback);

				return sol::nil;
			});
		}
	}
}