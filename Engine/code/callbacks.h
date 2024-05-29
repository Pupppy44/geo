#pragma once
#include <vector>
#include "../tree/callback.h"
#include "../util/tween.h"

namespace geo {
	namespace core {
		class game;
	}
	
	namespace code {
		// Class for managing Lua callbacks
		class callbacks {
		public:
			callbacks(geo::core::game*);

			// Call a callback based on its type
			void call_callback(tree::callback_type, sol::object);

			// Call an event callback
			void call_event(std::string, std::string, std::string = "");

			// Call an input callback
			void call_input(tree::callback_type, std::string);

			// Call a click callback
			void call_click(std::string, int, int);

			// Clear all callbacks
			void clear_callbacks();
		public:
			std::vector<tree::callback> calls;
			std::vector<tree::callback> events;
			std::vector<tree::callback> inputs;
			std::vector<tree::callback> clicks;
			std::vector<util::tween> tweens;
		private:
			geo::core::game* game = nullptr;
		};
	}
}