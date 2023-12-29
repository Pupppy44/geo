#include "../util/debugger.h"
#include "server.h"
#include "../util/logger.h"

namespace geo {
	namespace server {
		server::server() : game(), reflector(*this), players(*this), debugger() {
			game.server = this;
		};

		void server::load(std::string file) {
			// Load a game file into a string
			std::ifstream stream(file);
			std::string data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
			
			// Parse the game data
			game.parser.parse_game(data);

			DEBUG("loaded " + game.name + " (id = " + game.id + ") with " + std::to_string(game.engine.tree.get_objects().size()) + " objects");
		}

		void server::start(int port) {
			// Initiate server
			if (svr.init() != pascal::PASCAL_OK) {
				ERR("failed to initiate server");
				getchar();
				return;
			}
			
			// Start server
			if (svr.start(port) != pascal::PASCAL_OK) {
				ERR("failed to start server on port " + std::to_string(port));
				getchar();
				return;
			}
			
			DEBUG("started server (guid = '" + id + "') on port " + std::to_string(port));
			
#ifdef _DEBUG
			debugger.set("server_port", port);
			debugger.set("server_guid", id);
			debugger.set("game_name", game.name);
			debugger.set("game_id", game.id);
			debugger.set("game_color", game.window.get_hex_background());
			debugger.set("game_objects", std::to_string(game.engine.tree.get_objects().size()));
			debugger.set("game_players", std::to_string(players.get_players().size()));
			debugger.start(L"Geo Server Debugger");
#endif
			// Hook reflector to the game engine
			reflector.hook_server();

			// Start code environment
			game.runner.init();
			game.runner.start();

			// Hook handlers to their respective events
			svr.on_packet([&](pascal::peer p, pascal::packet_type type, std::string packet) {
				// Parse packet
				nlohmann::json pkt = nlohmann::json::parse(packet);

				if (type == pascal::PASCAL_PACKET_JOIN_GAME) {					
					// Join in the new player
					players.join_player(p, pkt[0]);
				}
			});

			// Hook handlers to their respective events
			svr.on_disconnect([&](pascal::peer p, std::string reason) {	
				auto player = players.get_player(p.guid);
			
				if (player != nullptr) {
					// Remove the player from the game
					players.remove_player(player, reason);
				}
			});

			getchar();
		}
		
		std::string server::get_access_key() {
			return getenv("GGS_ACCESS_KEY");
		}
	}
}