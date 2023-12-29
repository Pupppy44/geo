#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../tree/object.h"

#undef pascal
namespace pascal {
	struct peer;
}

namespace geo {
	namespace server {
		class server;
	}
	
	namespace network {
		// Server reflector for retrieving and sending reflections from/to clients
		class server_reflector {
		public:
			server_reflector(server::server& s) : server(&s) {};

			// Initiate the reflector by hooking into the server
			void hook_server();

			// Send game information to the client. Used when a player joins and before reflecting objects
			void send_game_info(pascal::peer);

			// Load all game objects to a client
			void load_game(pascal::peer);

			// Create an object to a client
			void create_object(pascal::peer, std::shared_ptr<tree::object>);

			// Reflect an object to clients
			void reflect_object(std::string, tree::property);

			// Fire an event to clients
			void fire_event(std::string, std::string);

			// Check if an object can be reflected to clients
			bool is_reflective(tree::object);
		private:
			server::server* server;
		};
	}
}

