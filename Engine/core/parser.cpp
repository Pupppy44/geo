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
			game->info.name = root.attribute("name").as_string("Untitled Game");
			// Game background as a hex color
			game->window.set_background(root.attribute("background").as_string("#000000"));
			// Game ID (if the game is online)
			game->info.id = root.attribute("id").as_string("-1");
			// Game thumbnail (shown in the game browser and menu)
			game->info.thumbnail = root.attribute("thumbnail").as_string("https://example.com");

			// Window width, height, and border mode
			game->window.set_size(root.attribute("width").as_int(1420), root.attribute("height").as_int(792), root.attribute("borderless").as_bool(false));

			// Loop through all objects in <geo>
			for (pugi::xml_node object = root.child("object"); object; object = object.next_sibling("object")) {
				// Object type
				std::string type = object.attribute("class").as_string();
				// Object properties
				std::vector<tree::property> properties = parse_properties(object);

				if (type == "") {
					continue;
				}

				// Parse the object separately 
				auto parsed_object = parse_object(type, properties);

				// Once parsed, we can add any child objects
				if (parsed_object) {
					for (pugi::xml_node child = object.child("object"); child; child = child.next_sibling("object")) {
						std::string child_type = child.attribute("class").as_string();
						std::vector<tree::property> child_properties = parse_properties(child);
						auto child_object = parse_object(child_type, child_properties);
						if (child_object) {
							parsed_object->add_child(child_object);
						}
					}
					
					game->engine.tree.add_object(parsed_object);
				}
			}

			return true;
		}

		std::shared_ptr<object> parser::parse_object(std::string type, std::vector<tree::property> properties) {
			std::shared_ptr<object> object;
			
			if (type == "audio") {
				object = std::make_shared<audio>();
			}
			else if (type == "script") {
				object = std::make_shared<script>();
			}
			else if (type == "event") {
				object = std::make_shared<network::event>(*game);
			}
			else if (type == "rectangle") {
				object = std::make_shared<rect>();
			}
			else if (type == "text") {
				object = std::make_shared<text>();
			}
			else if (type == "image") {
				object = std::make_shared<image>();
			}
			else if (type == "textbox") {
				object = std::make_shared<textbox>();
			}
			else if (type == "scrollbox") {
				object = std::make_shared<scrollbox>();
			}

			if (object) {
				object->set_properties(properties);
			}

			return object;
		}

		void parser::parse_objects(std::string data) {
			if (!doc.load_string(data.data())) {
				return;
			}

			// Get the root node (<geo>)
			pugi::xml_node root = doc.child("geo");
			if (!root) {
				return;
			}

			// Loop through all objects in <geo>
			for (pugi::xml_node object = root.child("object"); object; object = object.next_sibling("object")) {
				// Object type
				std::string type = object.attribute("class").as_string();
				// Object properties
				std::vector<tree::property> properties = parse_properties(object);

				// Parse the object separately 
				parse_object(type, properties);
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