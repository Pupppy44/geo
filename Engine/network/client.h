#pragma once
#include "client_reflector.h"
#include "pascal.hpp"
#include <nlohmann/json.hpp>
#include <future>

namespace geo {
	namespace core {
		class game;
	}
	
	namespace network {
		// Client class for connecting to a game server
		class client {
		public:
			client(core::game& game) : game(&game), reflector(game) {};
			
			// Initiate the client
			void init();

			// Connect to a game server
			bool connect(std::string, int);

			// Join an online game
			bool join_game(std::string);
		public:
			pascal::client cli;
			client_reflector reflector;
		private:
			core::game* game;
		};
	}
}

