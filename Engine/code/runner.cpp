#include "runner.h"
#include "../core/game.h"

namespace geo {
	namespace code {
		runner::runner(core::game& g) 
			: game(&g),
			registry(&g),
			callbacks(&g)
		{};

		runner::~runner() {};

		void runner::init() {
			lua.open_libraries(
				sol::lib::base,
				sol::lib::math,
				sol::lib::string,
				sol::lib::coroutine,
				sol::lib::table,
				sol::lib::utf8
			);

			// Register the tree, classes, functions, etc.
			registry.register_tree();
			registry.register_functions();
			registry.register_objects();
			registry.register_structs();
			registry.register_systems();
			registry.register_game_info();
			
			// TODO: Remove the preprocessor for this function, find a cleaner method
#ifdef SERVER
			registry.register_players();
#endif
		}

		void runner::start() {
			std::string all_scripts = "";
			
			for (auto& obj : game->engine.tree.get_objects()) {
				if (obj->type() == "script") {
					std::string script_context = obj->get_property<std::string>("context");

					// Run if the script type is the same as the context or default
					if (script_context == context || script_context == "default") {
						std::string code = obj->get_property<std::string>("code");
						all_scripts += "\n\n" + code;
					}
				}
			}

			run(all_scripts);
		}

		void runner::run(std::string code) {
			scripts++;

			// Run the code in a separate thread so it doesn't block the main thread
			std::jthread script_thread([=]() {
				try {
					lua.script(code);
				}
				catch (const sol::error& e) {
					// Log the error
					ERR(e.what());
				}
			});

			script_thread.detach();
		}

		void runner::clear() {
			// Clear callbacks
			callbacks.clear_callbacks();
			// Clear Lua environment
			lua = sol::state();
			// Reset the script count
			scripts = 0;
			// Reinitialize the runner
			init();
		}
	}
}