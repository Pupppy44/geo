#pragma once
#include "game_server.h"
#include "find_port.h"
#include "generate_guid.h"

namespace manager {
	// Creates a new game server
	inline game_server create_server(std::string id) {
		if (id == "") {
			id = "0";
		}

		// Setup server data
		game_server game;
		game.server_id = generate_guid();
		game.game_id = id;
		game.port = find_port();
		game.players = 0;

		// Start the server (arguments: game_id, port, server_id)
		const char* server_path = "G:\\server\\Geo Server.exe";
		game.pid = _spawnl(_P_NOWAIT, server_path, server_path, id.data(), std::to_string(game.port).data(), game.server_id.data(), nullptr);

		return game;
	}
}