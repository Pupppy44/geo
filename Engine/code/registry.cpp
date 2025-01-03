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
			text::bind<text>(game, lua);
			image::bind<image>(game, lua);
			textbox::bind<textbox>(game, lua);
			scrollbox::bind<scrollbox>(game, lua);
			audio::bind<audio>(game, lua);
			script::bind<script>(game, lua);
			network::player::bind<network::player>(game, lua);
			network::event::bind<network::event>(game, lua);
		}

		void registry::register_structs() {
			sol::state& lua = game->runner.lua;

			// Register all structures here using one bind call for each
			local_player::bind(lua);
			game_info::bind(lua);
		}

		void registry::register_systems() {
			sol::state& lua = game->runner.lua;
			
			// Register all systems here
			lua["geo"]["input_system"] = std::make_shared<input_system>(*game);
			input_system::bind<systems::input_system>(game, lua);

			lua["geo"]["http_system"] = std::make_shared<http_system>(*game);
			http_system::bind<systems::http_system>(game, lua);

			lua["geo"]["tween_system"] = std::make_shared<tween_system>(*game);
			tween_system::bind<systems::tween_system>(game, lua);

			lua["geo"]["data_system"] = std::make_shared<data_system>(*game);
			data_system::bind<systems::data_system>(game, lua);

			lua["geo"]["speech_system"] = std::make_shared<speech_system>(*game);
			speech_system::bind<systems::speech_system>(game, lua);
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
			lua.set_function("wait", [&](float secs, sol::function func) {
				std::jthread thread([=]() {
					std::this_thread::sleep_for(std::chrono::milliseconds(int(secs * 1000)));
					DEBUG("wait() waited " + std::to_string(secs) + " seconds");
					func();
				});

				thread.detach();
			});

			// Create an object
			lua["geo"]["create_object"] = [&](sol::this_state state, std::string type, std::string name) -> sol::object {
				auto obj = game->parser.parse_object(type, { { property_type::STRING, "name", name } });
				game->engine.tree.add_object(obj);
				
				// Initialize it here because it's not done in the parser
				obj->context = game->engine.get_context();
				obj->init();
				
				return sol::make_object(state, obj);
			};

			// Set a view
			lua["geo"]["set_view"] = [&](std::string view) {
				for (auto& obj : game->engine.tree.get_objects()) {
					std::string objectView = obj->get_property<std::string>("view");

					// Skip CoreUI objects and scripts
					if (
						obj->get_property<std::string>("name").contains("__CORE_UI_")
						|| obj->type() == "script"
						|| objectView == ""
						) {
						continue;
					}

					// Show the view, hide the other objects
					if (objectView == view) {
						property prop = tree::property(property_type::BOOLEAN, "hidden", "false");
						prop.updated = true;
						obj->set_property(prop);
					}
					else {
						property prop = tree::property(property_type::BOOLEAN, "hidden", "true");
						prop.updated = true;
						obj->set_property(prop);
					}
				}
			};
#ifdef CLIENT
			// Load a game from a Geo XML file
			lua["geo"]["load"] = [&](std::string xml) {
				game->start_local_game(xml);
				};

			// Exit the game and return to the main menu
			lua["geo"]["exit"] = [&]() {
				game->start_local_game(util::get_ui(1), false);
				};
#endif
		}

		void registry::register_game_info() {
			sol::state& lua = game->runner.lua;

			// Game info table
			lua["geo"]["info"] = &game->info;

			// Player info table
			lua["geo"]["player"] = &game->player;
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

			// Add callback for when a player joins
			lua["geo"]["players"]["on_join"] = [&](sol::variadic_args args) -> sol::object {
				// Create a callback object
				tree::callback callback;
				callback.type = callback_type::PLAYERS_ON_JOIN;
				callback.function = args[0].as<sol::function>();

				// Add the callback to the callbacks list 
				game->runner.callbacks.calls.push_back(callback);

				return sol::nil;
				};

			// Add callback for when a player leaves
			lua["geo"]["players"]["on_leave"] = [&](sol::variadic_args args) -> sol::object {
				// Create a callback object
				tree::callback callback;
				callback.type = callback_type::PLAYERS_ON_LEAVE;
				callback.function = args[0].as<sol::function>();

				// Add the callback to the callbacks list 
				game->runner.callbacks.calls.push_back(callback);

				return sol::nil;
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