#include "player_handler.h"
#include "server.h"
#include "../util/logger.h"

namespace geo {
	namespace server {
		// ! When authentication exists, we'll use tokens instead of names.
		// ! We'll also need an authentication function/helper for this.
		void player_handler::join_player(pascal::peer p, std::string name) {
			// Create a new player object
			auto plr = std::make_shared<network::player>(p, *server);

			// Set name
			plr->set_property(tree::property(tree::property_type::STRING, "name", name));

			// Add the player to the player list
			players.push_back(plr);

			// After registering, send game information to confirm the join
			server->reflector.send_game_info(p);

			// Once the join is confirmed, we can load all game objects to the player
			server->reflector.load_game(p);

			// We can also now call the on_join callbacks set by the server
			server->game.runner.callbacks.call_callback(tree::callback_type::PLAYERS_ON_JOIN, sol::make_object(server->game.runner.lua, plr));

			DEBUG("player " + name + " joined the game (guid = '" + p.guid + "')");
			server->debugger.set("game_players", std::to_string(players.size()));
		};

		void player_handler::remove_player(std::shared_ptr<network::player> plr, std::string reason) {
			// Remove the player from the player list
			players.erase(std::remove(players.begin(), players.end(), plr), players.end());

			// Disconnects the player if they're still connected
			server->svr.disconnect(plr->peer, reason);

			// We can also now call the on_leave callbacks set by the server
			server->game.runner.callbacks.call_callback(tree::callback_type::PLAYERS_ON_LEAVE, sol::make_object(server->game.runner.lua, plr));

			DEBUG("player (guid = '" + plr->peer.guid + "') removed (reason = '" + reason + "')");
			server->debugger.set("game_players", std::to_string(players.size()));
		};

		std::shared_ptr<network::player> player_handler::get_player(std::string id) {
			for (auto& plr : players) {
				// Check if the player's ID/GUID matches the given ID
				if (plr->peer.guid == id) {
					return plr;
				}
			}

			// Return an empty player
			return nullptr;
		};

		std::vector<std::shared_ptr<network::player>> player_handler::get_players() {
			return players;
		};
	}
}