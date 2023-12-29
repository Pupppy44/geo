#include "event.h"
#include "../core/game.h"
#include "../server/server.h"

namespace geo {
	namespace network {
		event::event(core::game& g) : game(&g) {
			type("event");

			// Fires the event to the server
			define("fire", [&](sol::variadic_args args) -> sol::object {
				std::string data = args[0].as<std::string>();

#ifdef SERVER
				// As a server, fire the event to the clients 
				game->server->reflector.fire_event(id(), data);
#else
				// As a client, fire the event to the server
				game->client.reflector.fire_event(id(), data);
#endif

				return sol::nil;
			});

			// Registers a callback for when the event is fired
			define("on_fire", [&](sol::variadic_args args) -> sol::object {
				// Create a callback object
				tree::callback callback;
				callback.name = get_property<std::string>("name");
				callback.id = id();
				callback.function = args[0].as<sol::function>();
				
				// Add the callback to the callbacks list 
				game->runner.callbacks.events.push_back(callback);

				return sol::nil;
			});
		};

		void event::init() {};

		void event::render() {};
	}
}