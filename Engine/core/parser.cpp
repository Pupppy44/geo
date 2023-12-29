#include "parser.h"
#include "game.h"
#include "../objects/objects.h"
using namespace geo::objects;

namespace geo {
	namespace core {
		parser::parser(core::game& game)
			: game(&game)
		{};

		bool parser::parse_game(std::string data) {
			if (!doc.load_string(data.data())) {
				return false;
			}

			// Get the root node (<geo>)
			pugi::xml_node root = doc.child("geo");
			if (!root) {
				return false;
			}

			// Game name, if any
			game->name = root.attribute("name").as_string("Untitled Game");
			// Game background as a hex color
			game->window.set_background(root.attribute("background").as_string("#000000"));
			// Game ID (if the game is online)
			game->id = root.attribute("id").as_string("-1");
			// Game thumbnail (shown in the game browser and menu)
			game->thumbnail = root.attribute("thumbnail").as_string("https://example.com");

			// Loop through all objects in <geo>
			for (pugi::xml_node object = root.child("object"); object; object = object.next_sibling("object")) {
				// Object type
				std::string type = object.attribute("class").as_string();
				// Object properties
				std::vector<tree::property> properties = parse_properties(object);

				// Parse the object separately 
				parse_object(type, properties);
			}

			return true;
		}

		void parser::parse_object(std::string type, std::vector<tree::property> properties) {
			if (type == "audio") {
				auto audio_object = std::make_shared<audio>();
				audio_object->set_properties(properties);
				game->engine.tree.add_object(audio_object);
			}
			else if (type == "script") {
				auto script_object = std::make_shared<script>();
				script_object->set_properties(properties);
				game->engine.tree.add_object(script_object);
			}
			else if (type == "event") {
				auto event_object = std::make_shared<network::event>(*game);
				event_object->set_properties(properties);
				game->engine.tree.add_object(event_object);
			}
			else if (type == "rectangle") {
				auto rect_object = std::make_shared<rect>();
				rect_object->set_properties(properties);
				game->engine.tree.add_object(rect_object);
			}
		}

		std::vector<tree::property> parser::parse_properties(pugi::xml_node object) {
			std::vector<tree::property> properties;

			// Loop through the property tags in the object node
			for (pugi::xml_node property = object.child("property"); property; property = property.next_sibling("property")) {
				// Property name as a string
				std::string name = property.attribute("name").as_string();
				// Property type (string/number/boolean)
				tree::property_type type = (tree::property_type)property.attribute("type").as_int();
				// Property value (the property constructor will convert the type if necessary)
				std::string value = property.attribute("value").value();
				
				// Add the property to the properties array
				properties.push_back(tree::property(type, name, value));
			}

			return properties;
		}
	}
}