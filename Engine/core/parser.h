#pragma once
#include "../tree/object.h"
#include <string>
#include <vector>
#include <pugixml.hpp>

namespace geo {
	namespace core {
		class game;
		
		class parser {
		public:
			parser(game&);

			// Parse and load a game file into the engine's tree
			bool parse_game(std::string);

			// Parse and load an object file into the engine's tree
			std::shared_ptr<tree::object> parse_object(std::string, std::vector<tree::property>);

			// Parse objects from a string
			void parse_objects(std::string);

			// Parse an object's properties into a properties array
			std::vector<tree::property> parse_properties(pugi::xml_node);
		private:
			core::game* game;
			pugi::xml_document doc = pugi::xml_document();
		};
	}
}