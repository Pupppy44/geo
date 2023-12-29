#pragma once
#include <string>
#include <vector>
#include <memory>
#include "./object.h"

namespace geo {
	namespace tree {
		// Game objects tree
		class tree {
		public:
			tree();
			~tree();

			// Add an object to the tree
			void add_object(std::shared_ptr<object> obj);

			// Remove an object from the tree
			void remove_object(std::shared_ptr<object> obj);

			// Get an object by its ID
			std::shared_ptr<object> get_object_by_id(std::string);

			// Get all objects
			std::vector<std::shared_ptr<object>> get_objects();

			// Clear all objects
			void clear_objects();
		private:
			std::vector<std::shared_ptr<object>> objects = {};
		};
	}
}