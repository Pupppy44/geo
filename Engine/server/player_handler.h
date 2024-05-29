#pragma once
#include <vector>
#include "../network/player.h"

namespace geo {
	namespace server {		
		// Player handler for managing connected/disconnected players
		class player_handler {
		public:
			player_handler(server& s) : server(&s) {};

			// Join a newly joined player
			void join_player(pascal::peer, std::string, std::string);

			// Remove a player from the player list
			void remove_player(std::shared_ptr<network::player>, std::string);

			// Get a player by their peer
			std::shared_ptr<network::player> get_player(std::string);

			// Get all players
			std::vector<std::shared_ptr<network::player>> get_players();
		private:
			server* server;
			std::vector<std::shared_ptr<network::player>> players;
		};
	}
}