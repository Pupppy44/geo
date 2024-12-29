#include "server_reflector.h"
#include "../server/server.h"
#include "../util/logger.h"

namespace geo {
	namespace network {
		void server_reflector::hook_server() {
			// Listen to different packets from clients
			server->svr.on_packet([&](pascal::peer peer, pascal::packet_type type, std::string packet) {
				if (type == pascal::PASCAL_PACKET_REFLECT) {
					// ...
				}
				else if (type == pascal::PASCAL_PACKET_REMOTE_EVENT) {
					// Parse remote event packet
					nlohmann::json evt = nlohmann::json::parse(packet);
					std::string id = evt[0];
					std::string data = evt[1];

					// Call any event callbacks with the player and data
					server->game.runner.callbacks.call_event(id, data, peer.guid);

					DEBUG("received remote event '" + id + "' with data '" + data + "' from client " + peer.guid);
				}
				else if (type == pascal::PASCAL_PACKET_CHAT) {
					auto player = server->players.get_player(peer.guid);
					if (player == nullptr || packet.empty()) return;

					// Create chat packet
					nlohmann::json chat;
					chat["name"] = player->get_property<std::string>("name");
					chat["message"] = packet;

					server->svr.broadcast(chat.dump(), pascal::PASCAL_PACKET_CHAT);
				}
			});

			// Run a loop that reflects updated properties
			// TODO: Might be better to add to the main engine/server loop
			std::jthread([&]() {
				while (true) {
					// Step all tweens
					auto& tweens = server->game.runner.callbacks.tweens;
					for (auto it = tweens.begin(); it != tweens.end();) {
						auto& tw = *it;

						float value = tw.step();
						tw.callback(value, tw.elapsed);

						// Remove tween callback from list if it's done
						if (tw.current_value == tw.target_value) {
							it = tweens.erase(it);
						}
						else {
							++it;
						}
					}
					
					for (auto& obj : server->game.engine.tree.get_objects()) {
						for (auto& prop : obj->get_properties()) {
							if (prop.updated) {
								// Reflect the new object's property if it's updated
								server->reflector.reflect_object(obj->id(), prop);

								// Reset the updated state
								prop.updated = false;
								obj->set_property(prop);
							}
						}
					}

					Sleep(1000 / 60);
				}
			}).detach();
		}

		void server_reflector::send_game_info(pascal::peer p) {
			// Information packet (name, id, background, thumbnail, width, height)
			nlohmann::json info_packet = nlohmann::json::array();
			info_packet.push_back(server->game.info.name);
			info_packet.push_back(server->game.info.id);
			info_packet.push_back(server->game.window.get_hex_background());
			info_packet.push_back(server->game.info.thumbnail);
			info_packet.push_back(server->game.info.width);
			info_packet.push_back(server->game.info.height);

			// Send GAME_JOINED packet with game information
			server->svr.send(p, info_packet.dump(), pascal::PASCAL_PACKET_GAME_JOINED);
		}

		void server_reflector::load_game(pascal::peer p) {
			// Send all objects to the client
			for (auto& object : server->game.engine.tree.get_objects()) {
				create_object(p, object);
			}

			// Done reflecting, game is loaded and ready to launch for the client
			server->svr.send(p, "", pascal::PASCAL_PACKET_GAME_LOADED);

			DEBUG("done reflecting " + std::to_string(server->game.engine.tree.get_objects().size()) + " objects");
		}

		void server_reflector::create_object(pascal::peer p, std::shared_ptr<tree::object> object) {
			if (!is_reflective(*object)) return;
			
			// Top object information + properties to be sent to the client
			nlohmann::json create = nlohmann::json::array();
			create.push_back(object->id());
			create.push_back(object->type());

			// Another array that contains the properties
			nlohmann::json props = nlohmann::json::array();
			for (auto& property : object->get_properties()) {
				// Format the property similar to game files: [name, type, value]
				nlohmann::json prop = { property.name, property.type, property.as_string() };
				props.push_back(prop);
			}
			
			// Add the properties array to the packet
			create.push_back(props);

			// Create!
			server->svr.send(p, create.dump(), pascal::PASCAL_PACKET_CREATE);

			DEBUG("created object " + object->id() + " of type " + object->type() + " with " + std::to_string(object->get_properties().size()) + " properties (for client '" + p.guid + "')");
		}

		// TODO: Just pass the object instead of id string
		void server_reflector::reflect_object(std::string id, tree::property property) {
			if (!is_reflective(*server->game.engine.tree.get_object_by_id(id))) return;
			
			// Reflection packet
			nlohmann::json reflect = nlohmann::json::array();
			reflect.push_back(id); // Object ID
			reflect.push_back(property.name); // Property name
			reflect.push_back(property.type); // Property type
			reflect.push_back(property.as_string()); // Property value
			
			// Reflect!
			server->svr.broadcast(reflect.dump(), pascal::PASCAL_PACKET_REFLECT);

			DEBUG("updated object property (object_id = '" + id + "'): " + property.name + " = " + property.as_string());
		}

		void server_reflector::fire_event(std::string id, std::string data) {
			// Form the remote event packet
			nlohmann::json evt = nlohmann::json::array();
			evt.push_back(id);
			evt.push_back(data);
			
			server->svr.broadcast(evt.dump(), pascal::PASCAL_PACKET_REMOTE_EVENT);
		}

		bool server_reflector::is_reflective(tree::object object) {
			// Don't reflect server scripts
			if (object.get_property<std::string>("context") == "server") {
				DEBUG("not reflecting server script (object_id = '" + object.id() + "')");
				
				return false;
			}
			
			return true;
		}
	}
}