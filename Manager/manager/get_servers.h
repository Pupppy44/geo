#pragma once
#include "game_server.h"
#include <string>
#include <nlohmann/json.hpp>

namespace manager {
	// Gets all the servers as a JSON string
	inline std::string get_servers(std::vector<game_server> game_servers) {
		if (game_servers.size() == 0) {
			return "{}";
		}
		
		nlohmann::json json;
		
		for (auto& server : game_servers) {
			json.push_back({
				{ "server_id", server.server_id },
				{ "port", server.port },
				{ "game_id", server.game_id },
				{ "players", server.players }
			});
		}

		return json.dump();
	}
}