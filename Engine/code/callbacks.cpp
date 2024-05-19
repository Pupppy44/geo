#include "callbacks.h"
#include "../core/game.h"
#include "../server/server.h"

namespace geo {
	namespace code {
		callbacks::callbacks(geo::core::game* g) : game(g) {};

		void callbacks::call_callback(tree::callback_type type, sol::object data) {
			for (auto& call : calls) {
				if (call.type == type) {
					call.function(data);
				}
			}
		}

		void callbacks::call_event(std::string id, std::string data, std::string player_id) {
			for (auto& event : events) {
				if (event.id == id) {
#ifdef SERVER
					// Call the event callback with the player and data
					auto player = game->server->players.get_player(player_id);
					event.function(player, data);
#else
					// Send the server's event data to the client callback
					event.function(data);
#endif
				}
			}
		}

		void callbacks::call_input(tree::callback_type type, std::string key) {
			for (auto& input : inputs) {
				if (input.type == type) {
					input.function(key);
				}
			}
		}
		
		void callbacks::clear_callbacks() {
			events.clear();
		}
	}
}