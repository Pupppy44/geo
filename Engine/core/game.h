#pragma once
#include <string>
#include "../code/runner.h"
#include "../network/client.h"
#include "../util/debugger.h"
#include "engine.h"
#include "window.h"
#include "parser.h"

namespace geo {
	namespace server {
		class server;
	}
	
	namespace core {
		// The main class for Geo games
		class game {
		public:
			game() :
				window(*this),
				engine(*this),
				runner(*this),
				parser(*this),
				client(*this),
				server(nullptr),
				debugger()
			{};

			// Initiate the game
			void init();

			// Leave the game
			void leave();

			// Load and start a local game from a file
			void start_local_game(std::string data);

			// Connect to an online game server
			void start_online_game(std::string ip, int port);
		public:
			// Game information
			std::string name;
			std::string id;
			std::string thumbnail;
		public:
			// Core components
			core::engine engine;
			core::window window;
			core::parser parser;
			code::runner runner;
			
			// Network components
			network::client client;
			server::server* server;

			// Debugger
			util::debugger debugger;
		};
	}
}
