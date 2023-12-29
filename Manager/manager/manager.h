#pragma once
#include "httplib.h"
#include "get_servers.h"
#include "create_server.h"
#include "../../Engine/util/logger.h"
#include <vector>

namespace manager {
	class manager {
	public:
		manager() {
			// Get: / - How'd you get here?
			server.Get("/", [](const httplib::Request&, httplib::Response& res) {
				res.set_content(";)", "text/plain");
			});

			// Get all servers
			server.Get("/servers", [&](const httplib::Request&, httplib::Response& res) {
				res.set_content(get_servers(game_servers), "application/json");
			});

			// GET: /create - Create a server using the given game ID
			server.Get("/create", [&](const httplib::Request& req, httplib::Response& res) {
#ifndef _DEBUG
				if (!req.remote_addr.starts_with("10.0")) {
					return;
				}
#endif

				// Start timer
				auto start = std::chrono::high_resolution_clock::now();
				
				// Create the server
				game_server game = create_server(req.get_param_value("id"));
				game_servers.push_back(game);

				LOG("created game server in " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count()) + " ms (server_id, game_id, port, pid) : " + game.server_id + ", " + game.game_id + ", " + std::to_string(game.port) + ", " + std::to_string(game.pid));

				// Server is ready by now, send the port
				res.set_content(std::to_string(game.port), "text/plain");
			});
		}
		
		void start() {
			LOG("starting server on port 80");

			server.listen("0.0.0.0", 80);
		}
	public:
		httplib::Server server;
		std::vector<game_server> game_servers;
	};
}