#pragma once
#include "../core/game.h"
#include "../network/server_reflector.h"
#include "../util/debugger.h"
#include "player_handler.h"
#include <fstream>

namespace geo {
	namespace server {			
		// Game server class
		class server {
		public:
			server();
			
			// Load a game using its data
			void load(std::string);

			// Start the game server at a given port
			void start(int);
		public:
			// The game!
			core::game game;
			// Server ID
			std::string id;
			// Server networking
			pascal::server svr;
			// Player handler for managing players and their connections
			player_handler players;
			// Network reflector
			network::server_reflector reflector;
			// Debugger
			util::debugger debugger;
		private:
			// Retrieve the game server access key
			std::string get_access_key();
		};
	}
}
