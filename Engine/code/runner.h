#pragma once
#include <string>
#include <sol/sol.hpp>
#include "registry.h"
#include "callbacks.h"

namespace geo {
	namespace core {
		class game;
	}
	
	namespace code {
		class runner {
		public:
			runner(core::game&);
			~runner();

			void init();
			
			// Executes all scripts in a loaded game's tree
			void start();

			// Executes a script as a coroutine
			void run(std::string, sol::coroutine);

			// Clear the Lua environment
			void clear();
		public:
			core::game* game;
			
			code::registry registry;
			code::callbacks callbacks;
			
			// Lua environment
			sol::state lua;
			
			// Number of scripts executed
			int scripts = 0;

			// Runner context (client or server, uses preprocessor to determine)
#ifdef CLIENT
			const std::string context = "client";
#else
			const std::string context = "server";
#endif
		};
	}
}