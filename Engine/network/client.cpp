#include "client.h"
#include "../core/game.h"
#include "../util/logger.h"

namespace geo {
	namespace network {
		void client::init() {
			reflector.hook_client();
		}

		bool client::connect(std::string ip, int port) {
			DEBUG("connecting to " + ip + ":" + std::to_string(port));
			
			if (cli.init() == pascal::PASCAL_INIT_FAILED)
				return false;

			if (cli.connect(ip, port) != pascal::PASCAL_OK)
				return false;

			// Start a connection timer
			auto connection_timer = std::chrono::high_resolution_clock::now();

			// Wait for a successful connection using a loop
			while (true) {
				// Check if the connection is established
				if (cli.connected) {
					DEBUG("connected to game");
					
					game->debugger.set("online_server_ip", ip);
					game->debugger.set("online_server_port", port);

					return true;
				}

				// Check if the connection timed out
				if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - connection_timer).count() >= 10) {
					// Failed!
					ERR("couldn't connect to " + ip + ":" + std::to_string(port));
					return false;
				}

				Sleep(1000 / 60);
			}
		}
		
		bool client::join_game(std::string name) {
			DEBUG("joining as " + name);

			// Create the join game packet
			nlohmann::json join_packet = nlohmann::json::array();
			join_packet.push_back(name); // Name/ID of client

			// Listener for game information and load confirmation
			bool loaded = false;
			cli.on_packet([&](pascal::packet_type type, std::string packet) {
				if (type == pascal::PASCAL_PACKET_GAME_JOINED) {
					// Parse game information packet
					nlohmann::json game_info = nlohmann::json::parse(packet);

					// Set game information
					game->name = game_info[0];
					game->id = game_info[1];
					game->window.set_background(game_info[2]);
					game->thumbnail = game_info[3];
				}
				else if (type == pascal::PASCAL_PACKET_GAME_LOADED) {
					DEBUG("joined game as " + name + " (client_id = '" + cli.get_guid() + "')");

					game->debugger.set("game_type", std::string("online"));
					game->debugger.set("online_username", name);
					game->debugger.set("online_client_guid", cli.get_guid());

					loaded = true;
				}
			});

			// Send packet to join game
			cli.send(join_packet.dump(), pascal::PASCAL_PACKET_JOIN_GAME);

			// Wait for the game to be loaded
			while (!loaded) {
				Sleep(1000 / 60);
			}

			return true;
		}
	}
}