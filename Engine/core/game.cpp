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
			window.init(1920, 1080, false);

			// Initiate the engine
			engine.init();
			
			// Initiate the runner
			runner.init();
		}

		void game::clear() {
			// Destroy the engine to a new state
			engine.clear();
			// Clear the runner
			runner.clear();
		}

		void game::start_local_game(std::string file, bool load_core_ui) {
			debugger.set("game_type", std::string("local"));

			// Clear the current game state
			clear();

			// Detect if the file is a path or XML data
			if (file.find("<") == std::string::npos) {
				std::ifstream t(file);
				std::string data((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
				file = data;
			}

			// Parse the game from the file
			parser.parse_game(file);
			
			// Load the Core UI
			if (load_core_ui) {
				parser.parse_objects(util::get_ui(UI_CORE_UI));
			}
		
			// Set the game/window title
			window.set_title(info.name + " - Geo Client (Beta)");
				
			// Start the game loop (including any scripts)
			window.start();
		}

		void game::start_online_game(std::string ip, int port) {
			// Clear the current game state
			clear();
			
			// Initiate the client and its reflector
			client.init();
			
			// Connect to the server
			if (!client.connect(ip, port)) {				
				MessageBox(NULL, L"The game server you're trying to reach is either nonexistent or having some problems. Sorry!", L"Connection Timeout", MB_ICONERROR | MB_OK);
				
				exit(0);
			}

			// Join and load the online game
			client.join_game();

			// Update window size
			window.set_size(info.width, info.height);

			// Start the game loop
			window.start();
		}
	}
}