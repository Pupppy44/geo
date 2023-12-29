#pragma once
#include <string>

namespace manager {
	struct game_server {
		int pid;
		int port;
		std::string server_id;
		std::string game_id;
		int players;
	};
}