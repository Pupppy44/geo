#include "tween_system.h"
#include "../core/game.h"
#include "../helpers/tween.h"
using namespace geo::util;

namespace geo {
	namespace systems {
		tween_system::tween_system(core::game& g) : game(&g) {
			type("tween_system");

			define("create", [&](sol::variadic_args args) -> sol::object {
				float initial = args[0].as<float>();
				float target = args[1].as<float>();
				float duration = args[2].as<float>();
				sol::function callback = args[3].as<sol::function>();
				
				// Create the tween object
				helpers::tween t(initial, target, duration, helpers::tween::easing::sine_ease_in_out);
				t.callback = [=](float value, float elapsed) {
					callback(value, elapsed);
				};

				// Add to callbacks list
				game->runner.callbacks.tweens.push_back(t);

				return sol::nil;
			});
		}
	}
}