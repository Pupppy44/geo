#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <iostream>
#include "../core/game.h"

namespace geo {
	namespace helpers {
		// Animation class for setting up and running animations
		class animations {
		public:
			struct animation_options {
				std::vector<std::pair<std::string, std::string>> _options;

				std::string get(std::string key, std::string default_option) {
					for (auto& option : _options) {
						if (option.first == key) {
							return option.second;
						}
					}

					return default_option;
				}

				void set(std::string key, std::string value) {
					for (auto& option : _options) {
						if (option.first == key) {
							option.second = value;
							return;
						}
					}

					_options.push_back({ key, value });
				}
			};

			animations() {
				// Registering each animation
				register_animation("zoom", &animations::_run_zoom_animation);
				register_animation("fly_in", &animations::_run_fly_in_animation);
			}

			// Run an animation
			void run_animation(std::string animation_name, std::shared_ptr<tree::object> object) {
				auto it = animations_map.find(animation_name);
				if (it != animations_map.end()) {
					// Call the function associated with the name
					(this->*(it->second))(object, animation_options());

					DEBUG("running " + animation_name + " animation for object (id=" + object->id() + ")");
				} else {
					WARN("animation not found: " + animation_name + "");
				}
			}
		private:
			// Stores the animation functions
			std::unordered_map<std::string, void (animations::*)(std::shared_ptr<tree::object>, animation_options)> animations_map;
		private:
			// Register the animations
			void register_animation(const std::string& name, void (animations::* func)(std::shared_ptr<tree::object>, animation_options)) {
				animations_map[name] = func;
			}

			// Private animation functions /// 

			void _run_zoom_animation(std::shared_ptr<tree::object> object, animation_options options) {
				float initial_width = (options.get("type", "in") == "in") ? 0.0f : object->get_property<float>("width");
				float initial_height = (options.get("type", "in") == "in") ? 0.0f : object->get_property<float>("height");
				float target_width = (options.get("type", "in") == "in") ? object->get_property<float>("width") : 0.0f;
				float target_height = (options.get("type", "in") == "in") ? object->get_property<float>("height") : 0.0f;

				// center, top_left, top_right, bottom_left, bottom_right
				std::string zoom_origin = options.get("origin", "bottom_right");

				float initial_x = object->get_property<float>("x");
				float initial_y = object->get_property<float>("y");

				helpers::tween t(initial_width, target_width, std::stof(options.get("duration", "500")), helpers::tween::easing::sine_ease_in_out);

				t.callback = [=](float current_width, float elapsed) {
					// Interpolate height proportionally
					float current_height = initial_height + (target_height - initial_height) * (current_width / target_width);

					// Calculate the change in width and height
					float delta_width = current_width - initial_width;
					float delta_height = current_height - initial_height;

					float new_x = initial_x;
					float new_y = initial_y;

					if (zoom_origin == "top_left") {
						__noop; // Is it cool to use this? (no)
					}
					else if (zoom_origin == "top_right") {
						new_x = initial_x - delta_width;
					}
					else if (zoom_origin == "bottom_left") {
						new_y = initial_y - delta_height;
					}
					else if (zoom_origin == "bottom_right") {
						new_x = initial_x - delta_width;
						new_y = initial_y - delta_height;
					}
					else if (zoom_origin == "center") { 
						new_x = initial_x - delta_width / 2.0f;
						new_y = initial_y - delta_height / 2.0f;
					}

					object->set_property("x", new_x);
					object->set_property("y", new_y);
					object->set_property("width", current_width);
					object->set_property("height", current_height);

					DEBUG("helpers::animations: zooming object (id=" + object->id() + ") to " + std::to_string(current_width) + "x" + std::to_string(current_height) + " at " + std::to_string(new_x) + "," + std::to_string(new_y));
					};

				object->game->runner.callbacks.tweens.push_back(t);
			}

			void _run_fly_in_animation(std::shared_ptr<tree::object> object, animation_options options) {
				// :) 
			}
		};
	}
}
