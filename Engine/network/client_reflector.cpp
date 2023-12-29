#include "client_reflector.h"
#include "../core/game.h"
#include "../util/logger.h"

namespace geo {
	namespace network {
		// TODO: Split into two functions: hook_create and hook_reflect
		void client_reflector::hook_client() {
			// Listen to reflection packets from the server
			game->client.cli.on_packet([&](pascal::packet_type type, std::string packet) {
				// Create
				if (type == pascal::PASCAL_PACKET_CREATE) {
					// Parse the new object from the server
					nlohmann::json create = nlohmann::json::parse(packet);
					
					// Object information (unique ID, class type, and properties array)
					std::string id = create[0];
					std::string class_type = create[1];
					nlohmann::json props = create[2];

					std::vector<tree::property> properties;

					// Iterate through the properties and add them to the properties vector
					for (auto& prop : props) {
						// Parse the property from the JSON
						std::string prop_name = prop[0];
						tree::property_type prop_type = (tree::property_type)prop[1];
						std::string prop_value = prop[2];

						// Create the property object and add it to the properties vector
						tree::property property(prop_type, prop_name, prop_value);
						properties.push_back(property);
					}

					// We can now use the parser to parse the object
					game->parser.parse_object(class_type, properties);

					// Set ID of the object (last object is the one we just created)
					// TODO: Make parse_object just return the object instead? 
					game->engine.tree.get_objects().back()->id(id);

					DEBUG("received new object " + id + " of type " + class_type + " with " + std::to_string(properties.size()) + " properties");
				}
				// Reflect
				else if (type == pascal::PASCAL_PACKET_REFLECT) {
					// Parse the reflection
					nlohmann::json reflect = nlohmann::json::parse(packet);

					// Reflection information
					std::string id = reflect[0];
					std::string prop_name = reflect[1];
					tree::property_type prop_type = (tree::property_type)reflect[2];
					std::string prop_value = reflect[3];

					// Update the property for the corresponding object
					for (auto& obj : game->engine.tree.get_objects()) {
						if (obj->id() == id) {
							// Create and set the property struct
							tree::property prop(prop_type, prop_name, prop_value);
							obj->set_property(prop);

							DEBUG("updated object property (id = '" + id + "'): " + prop_name + " = " + prop_value);
						}
					}
				}
				// Remote Event
				else if (type == pascal::PASCAL_PACKET_REMOTE_EVENT) {
					// Parse the event
					nlohmann::json evt = nlohmann::json::parse(packet);

					// Event information
					std::string id = evt[0];
					std::string data = evt[1];

					// Call all event callbacks
					game->runner.callbacks.call_event(id, data);

					DEBUG("received remote event: " + id + " (data = " + data + ")");
				}
			});

			// Listen to disconnects from the server
			game->client.cli.on_disconnect([&](std::string reason) {
				DEBUG("player disconnected (reason: " + reason + ")");
				MessageBoxA(NULL, reason.c_str(), "Disconnected", MB_OK | MB_ICONERROR);
				game->leave();
			});
		}

		void client_reflector::fire_event(std::string id, std::string data) {
			// Form the remote event packet
			nlohmann::json evt = nlohmann::json::array();
			evt.push_back(id);
			evt.push_back(data);

			game->client.cli.send(evt.dump(), pascal::PASCAL_PACKET_REMOTE_EVENT);
		}
	}
}