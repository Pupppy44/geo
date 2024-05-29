#pragma once
#include <string>
#include "../code/runner.h"
#include "../network/client.h"
#include "../util/debugger.h"
#include "../structs/structs.h"
#include "engine.h"
#include "window.h"
#include "parser.h"
#ifdef CLIENT
#include "../../Client/resource.h"
#else
#include "../../Server/resource.h"
#endif

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

			// Clear the game
			void clear();

			// Load and start a local game from a file
			void start_local_game(std::string data, bool load_core_ui = true);

			// Connect to an online game server
			void start_online_game(std::string ip, int port);
		public:
			// Game information
			game_info info;
			
			// Player information
			local_player player;
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
