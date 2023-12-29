#pragma once
#include "../tree/object.h"
#include "pascal.hpp"

namespace geo {
	namespace server {
		class server;
	}
	
	namespace network {
		class player : public tree::object {
		public:
			player(pascal::peer, server::server&);
			
			void init();
			void render();
		public:
			pascal::peer peer;
			server::server* server;
		};
	}
}