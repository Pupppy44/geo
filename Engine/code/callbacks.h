#pragma once
#include <vector>
#include "../tree/callback.h"

namespace geo {
	namespace core {
		class game;
	}
	
	namespace code {
		// Class for managing Lua callbacks
		class callbacks {
		public:
			callbacks(geo::core::game*);

			// Call an event callback
			void call_event(std::string, std::string, std::string = "");

			// Call an input callback
			void call_input(tree::callback_type, std::string);

			// Clear all callbacks
			void clear_callbacks();
		public:
			std::vector<tree::callback> events;
			std::vector<tree::callback> inputs;
		private:
			geo::core::game* game = nullptr;
		};
	}
}