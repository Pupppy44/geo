#include "animation_system.h"
#include "../core/game.h"

namespace geo {
    namespace systems {
        animation_system::animation_system(core::game& g) : game(&g) {
            type("animation_system");

			// Create a path given a list of x,y|x,y|x,y... coordinates
            define("create_path", [&](sol::variadic_args args) -> sol::object {
				std::shared_ptr<object> object = args[0].as<std::shared_ptr<tree::object>>();
				std::string data = args[1].as<std::string>();

				// Create/run and return the path
				std::vector<std::pair<float, float>> path = create_path(object, data);
				sol::table table_path = path_to_table(args.lua_state(), path);

				return table_path;
            });
        }

		void animation_system::render() {
			render_paths();
			render_animations();
		}

		void animation_system::render_paths() {
			for (auto& path : paths) {
				if (path.points.size() > 0) {
					// Move the object to the next point (aka. first point)
					auto& point = path.points[0];

					DEBUG("pathing object (id=" + path.object->id() + ") to " + std::to_string(point.first) + "," + std::to_string(point.second));

					path.object->set_property({
						tree::property_type::NUMBER,
						"x",
						std::to_string(point.first)
					});

					path.object->set_property({
						tree::property_type::NUMBER,
						"y",
						std::to_string(point.second)
					});

					// Remove the point so we can move to the next one
					path.points.erase(path.points.begin());
				}
				else {
					// Remove the path from the list (aka. removing empty paths)
					paths.erase(std::remove_if(paths.begin(), paths.end(), [&](animation_system::path p) {
						return p.points.size() == 0;
					}), paths.end());

					DEBUG("pathing object (id=" + path.object->id() + ") finished");
				}
			}
		}

		void animation_system::render_animations() {
			for (auto& animation : animations) {}
		}

		std::vector<std::pair<float, float>> animation_system::create_path(std::shared_ptr<object> object, std::string data) {
			std::vector<std::pair<float, float>> path_data;

			// Split function
			auto split = [](std::string s, char delim) {
				std::vector<std::string> result;
				std::stringstream ss(s);
				std::string item;
				while (std::getline(ss, item, delim)) {
					result.push_back(item);
				}
				return result;
			};

			// Split the data into pairs (x,y|x,y|x,y|...)
			auto pairs = split(data, '|');
			for (auto& pair : pairs) {
				auto xy = split(pair, ',');
				path_data.push_back({ std::stof(xy[0]), std::stof(xy[1]) });
			}

			// Path will be handled by the engine loop
			animation_system::path path = {
				path_data,
				object
			};
			paths.push_back(path);
			
			return path_data;
		}

		sol::table animation_system::path_to_table(lua_State* state, std::vector<std::pair<float, float>> path) {
			sol::table table = sol::state_view(state).create_table();

			// An array of {x = x, y = y} tables
			for (int i = 0; i < path.size(); i++) {
				sol::table xy = sol::state_view(state).create_table();
				xy["x"] = path[i].first;
				xy["y"] = path[i].second;

				table[i + 1] = xy;
			}

			return table;
		}
    }
}