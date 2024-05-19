#include "game.h"
#include "../server/server.h"
#include "../util/logger.h"
#include <memory>
#include <thread>

namespace geo {
	namespace core {
		void game::init() {
			// Initiate COM
			CoInitializeEx(NULL, COINIT_MULTITHREADED);
			
			// Initiate the window
			window.init(800, 600, false);

			// Initiate the engine
			engine.init();
			
			// Initiate the runner
			runner.init();
		}

		void game::leave() {
			// Destroy the engine to a new state
			engine.destroy();
		}

		void game::start_local_game(std::string file) {
			debugger.set("game_type", std::string("local"));

			// Get data from file
			std::ifstream t(file);
			std::string data((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			
			// Parse the game
			parser.parse_game(data);

			// Set the game/window title
			window.set_title(name + " - Geo Client (Beta)");
				
			// Start the game loop (including any scripts)
			window.start();
		}

		void game::start_online_game(std::string ip, int port) {
			// Initiate the client and its reflector
			client.init();
			
			// Connect to the server
			if (!client.connect(ip, port)) {				
				MessageBox(NULL, L"The game server you're trying to reach is either nonexistent or having some problems. Sorry!", L"Connection Timeout", MB_ICONERROR | MB_OK);
				
				exit(0);
			}

			// Join and load the online game
			client.join_game("Guest");
			
			// Execute loaded scripts
			runner.start();

			// Start the game loop
			window.start();
		}
	}
}