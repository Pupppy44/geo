#pragma once
#include <string>

namespace geo {
	namespace core {
		class game;
	}
	
	namespace network {
		class client_reflector {
		public:
			client_reflector(core::game& game) : game(&game) {};

			// Initiate the reflector by hooking into the client
			void hook_client();

			// Fire an event to the server
			void fire_event(std::string, std::string);
		private:
			core::game* game;
		};
	}
}

