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
			for (auto& obj : game->engine.tree.get_objects()) {
				if (obj->type() == "script") {
					std::string name = obj->get_property<std::string>("name");
					std::string script_context = obj->get_property<std::string>("context");

					if (script_context == context || script_context == "default") {
						std::string code = obj->get_property<std::string>("code");
						try {
							sol::protected_function pf = lua.load(code);
							sol::coroutine cr = pf;

							// Run the script
							run(name, cr);
						}
						// Syntax errors
						catch (sol::error err) {
							std::string what = err.what();
							ERR("Script " + name + " had a syntax error: " + what);
						}
						
					}
				}
			}
		}

		void runner::run(std::string name, sol::coroutine cr) {
			scripts++;

			if (cr.status() == sol::call_status::yielded) {
				// resume the coroutine
				
				
				auto result = cr();

				// Check for errors
				if (!result.valid()) {
					sol::error err = result;
					std::string what = err.what();
					ERR("Script " + name + " had an error: " + what);
				}
				else {
					LOG("Script " + name + " had no errors.");
				}
			}
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