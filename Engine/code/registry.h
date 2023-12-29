#pragma once
#include <sol/sol.hpp>

namespace geo {
	namespace core {
		class game;
	}
	
	namespace code {
		// Code registry for binding C++ classes to Lua
		class registry {
		public:
			registry(geo::core::game*);
			
			// Register all classes
			void register_objects();

			// Register all systems
			void register_systems();
			
			// Register the objects tree
			void register_tree();

			// Register the core functions
			void register_functions();

#ifdef SERVER
			// Register the players object
			void register_players();
#endif
		private:
			geo::core::game* game = nullptr;
		};
	};
}

