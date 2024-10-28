#pragma once
#include "hook.h"
#include "server/server.h"
#include "view.h"

namespace studio {
	// Core class for Geo Studio that manages the window, engine, and other components
	class studio {
	public:
		studio() : game(), server(), window(), hook() {};
		
		// Initialize the studio
		void init();
		
		// Start the studio
		void start();
	private:
		// Engine components
		geo::core::game game;
		geo::server::server server;

		// Studio components
		view window;
		hook hook;
	};
}
