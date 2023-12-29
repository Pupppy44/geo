#include "../core/game.h"
#include "../objects/objects.h"
#include "../systems/systems.h"
#include "../server/server.h"

namespace geo {
	namespace code {
		registry::registry(geo::core::game* g)
			: game(g)
		{};

		void registry::register_objects() {
			sol::state& lua = game->runner.lua;

			// Register all objects here using one bind call for each
			object::bind<tree::object>(game, lua);
			rect::bind<rect>(game, lua);
			audio::bind<audio>(game, lua);
			script::bind<script>(game, lua);
			network::player::bind<network::player>(game, lua);
			network::event::bind<network::event>(game, lua);
		}

		void registry::register_systems() {
			sol::state& lua = game->runner.lua;
			
			// Register all systems here
			lua["geo"]["input_system"] = std::make_shared<input_system>(*game);
			input_system::bind<systems::input_system>(game, lua);

			// Register all systems here
			lua["geo"]["http_system"] = std::make_shared<http_system>(*game);
			http_system::bind<systems::http_system>(game, lua);
		}

		void registry::register_tree() {
			sol::state& lua = game->runner.lua;

			// Global namespace and game tree
			lua["geo"] = lua.create_table();
			lua["geo"]["game"] = lua.create_table();

			// Turn the game into a metatable so objects can be accessed
			sol::table metatable = lua.create_table();
			metatable[sol::meta_function::index] = [&](sol::this_state state, sol::table table, std::string key) -> sol::object {
				for (auto& obj : game->engine.tree.get_objects()) {
					if (obj->get_property<std::string>("name") == key) {
						return sol::make_object(state, obj);
					}
				}

				// Return nil if the object doesn't exist
				return sol::lua_nil;
				};

			// Set the game tree metatable
			lua["geo"]["game"][sol::metatable_key] = metatable;
		}

		void registry::register_functions()  {
			sol::state& lua = game->runner.lua;

			// Print to the console
			lua.set_function("print", [](sol::variadic_args args) {
				for (int i = 0; i < args.size(); i++) {
					switch (args[i].get_type()) {
					case sol::type::number:
						LOG(std::to_string(args[i].as<float>()));
						break;
					case sol::type::string:
						LOG(args[i].as<std::string>());
						break;
					case sol::type::boolean:
						LOG(std::string(args[i].as<bool>() == true ? "true" : "false"));
						break;
					case sol::type::nil:
						LOG("nil");
					}
				}
			});

			// Wait for a certain amount of seconds
			lua.set_function("wait", [&](float secs) {				
				std::this_thread::sleep_for(std::chrono::milliseconds((int)(secs * 1000)));
			});
		}

#ifdef SERVER
		void registry::register_players() {
			sol::state& lua = game->runner.lua;

			// Players table
			lua["geo"]["players"] = lua.create_table();

			// Add get function for getting a player by name
			lua["geo"]["players"]["get_players"] = [&]() {
				return sol::make_object(lua, game->server->players.get_players());
				};

			// Add index metamethod for accessing players
			sol::table metatable = lua.create_table();
			metatable[sol::meta_function::index] = [&](sol::this_state state, sol::table table, std::string key) -> sol::object {
				for (auto& plr : game->server->players.get_players()) {
					if (plr->get_property<std::string>("name") == key) {
						return sol::make_object(state, plr);
					}
				}

				// Return nil if the object doesn't exist
				return sol::lua_nil;
				};

			// Add length metamethod for getting the number of players with the # prefix
			metatable[sol::meta_function::length] = [&]() -> int {
				size_t size = game->server->players.get_players().size();
				return static_cast<int>(size);
				};

			// Set the players table metatable
			lua["geo"]["players"][sol::metatable_key] = metatable;
		}
#endif
	}
}