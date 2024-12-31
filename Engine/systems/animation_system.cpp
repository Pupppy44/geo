#include "animation_system.h"
#include "../helpers/animations.h"

namespace geo {
    namespace systems {
        animation_system::animation_system(core::game& g) : game(&g) {
            type("animation_system");

			// Initialize the animation helper
			animation_helper = new helpers::animations();

			// Create a path given a list of x,y|x,y|x,y... coordinates
            define("create_path", [&](sol::variadic_args args) -> sol::object {
				std::shared_ptr<object> object = args[0].as<std::shared_ptr<tree::object>>();
				std::string data = args[1].as<std::string>();

				// Create/run and return the path
				std::vector<std::pair<float, float>> path = create_path(object, data);
				sol::table table_path = path_to_table(args.lua_state(), path);

				return table_path;
            });

			// Create an animation
			define("create_animation", [&](sol::variadic_args args) -> sol::object {
				std::shared_ptr<object> object = args[0].as<std::shared_ptr<tree::object>>();
				std::string type = args[1].as<std::string>();

				// Create the animation
				animation a = {
					.type = type,
					.object = object
				};
				animations.push_back(a);

				return sol::nil;
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
			for (auto& animation : animations) {
				animation_helper->run_animation(animation.type, animation.object);
			}
			animations = {};
		}

        std::vector<std::pair<float, float>> animation_system::create_path(std::shared_ptr<object> object, std::string data) {
            std::vector<std::pair<float, float>> path_data;
			std::vector<std::pair<float, float>> original_path_data; // For relative movement

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

            // Split the data into mode and path data
            auto split_data = split(data, ' ');
            if (split_data.size() != 2 || (split_data[0] != "origin" && split_data[0] != "absolute")) {
                ERR("invalid path data: " + data);
                return path_data;
            }
            std::string mode = split_data[0];
            data = split_data[1];

            // Split the path data into pairs (x,y|x,y|x,y|...)
            auto pairs = split(data, '|');
			int index = 0;
            for (auto& pair : pairs) {
                auto xy = split(pair, ',');

                if (xy.size() != 2) {
                    ERR("invalid path data: " + data);
                    return path_data;
                }

                // Parse the x, y coordinates
                float x = std::stof(xy[0]);
                float y = std::stof(xy[1]);

                if (mode == "origin") {
                    float object_x = object->get_property<float>("x");
                    float object_y = object->get_property<float>("y");

                    if (path_data.empty()) {
                        // First point is the object's position (so path starts from the object)
                        path_data.push_back({ object_x, object_y });
						original_path_data.push_back({ x, y });
                    }
                    else {
						original_path_data.push_back({ x, y });

						float x_offset = original_path_data[index].first - original_path_data[index - 1].first;
						float y_offset = original_path_data[index].second - original_path_data[index - 1].second;

						// Calculate the previous point's position
						float prev_x = path_data[index - 1].first;
						float prev_y = path_data[index - 1].second;

						// Add the offset to the previous point's position
						path_data.push_back({ prev_x + x_offset, prev_y + y_offset });
                    }
                    index++;
                }
                else {
                    path_data.push_back({ x, y });
                }
            }

            // Path will be handled by the engine loop
            animation_system::path path = {
                .points = path_data,
                .object = object
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