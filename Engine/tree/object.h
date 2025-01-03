#pragma once
#include "../util/helpers.h"
#include "../util/logger.h"
#include "./property.h"
#include "./function.h"
#include "./callback.h"

#include <string>
#include <vector>
#include <sol/sol.hpp>
#include <typeindex>
#define _WINSOCKAPI_
#include <d2d1_3.h>

namespace geo {
	namespace core {
		class game;
	}
	
	namespace tree {
		class object {
		public:
			// Rendering
			virtual void init() {};
			virtual void render() {};
			virtual void message(UINT, WPARAM, LPARAM) {};

			// Types
			std::string type(std::string t = "") {
				if (t != "") {
					_type = t;
					setup();
				}

				return _type;
			}

			// IDs
			std::string id(std::string i = "") {
				if (i != "") {
					_id = i;
				}

				return _id;
			}

			// Binds the object and its properties to Lua
			template<typename T>
			static void bind(core::game* game, sol::state& lua) {
				lua.new_usertype<T>(typeid(T).name(),
					// Get property
					"__index", [&](T& obj, const std::string& name) -> sol::object {
						// Properties lookup
						for (property& property : obj.get_properties()) {
							if (property.name == name) {
								DEBUG("__index: Get property " + name + " with value " + property.as_string());

								// Return the property value based on its type
								switch (property.type) {
								case property_type::NUMBER:
									return sol::make_object(lua, property.get<float>());
								case property_type::STRING:
									return sol::make_object(lua, property.get<std::string>());
								case property_type::BOOLEAN:
									return sol::make_object(lua, property.get<bool>());
								}
							}
						}
						
						// Function lookup
						for (function& func : obj.get_functions()) {
							if (func.name == name) {
								DEBUG("__index: Called function " + name);

								return sol::make_object(lua, func.function);
							}
						}

						// Return nil if we can't find anything
						return sol::nil;
					},
					// Set property
					"__newindex", [&](object& obj, const std::string& name, sol::object value) {
						// Properties assignment
						property new_property;

						if (value.is<float>()) {
							new_property = property(property_type::NUMBER, name, std::to_string(value.as<float>()));
						}
						else if (value.is<std::string>()) {
							new_property = property(property_type::STRING, name, value.as<std::string>());
						}
						else if (value.is<bool>()) {
							new_property = property(property_type::BOOLEAN, name, std::string(value.as<bool>() ? "true" : "false"));
						}

						// Update the object
						new_property.updated = true;

						// Set the property
						// If it already exists, it will just update the value
						obj.set_property(new_property);

						DEBUG("__newindex: Set property " + name + " to " + new_property.as_string());

						return;
					}
				);
			};

			// Define a function to the object's Lua functions
			void define(std::string, std::function<sol::object(sol::variadic_args)>);

			// Setup any properties or functions
			void setup();
			
			// Add a callback
			void add_callback(callback_type, sol::function);

			// Call a callback
			void call(callback_type, std::vector<sol::object>);

			// Property methods
			property get_property(std::string name);
			
			template <typename T>
			T get_property(std::string name, T default_value = T()) {
				for (property& prop : properties) {
					if (prop.name == name) {
						// Get the actual value instead of a property object
						return prop.get<T>();
					}
				}

				return default_value;
			}
			
			void set_property(property prop);

			std::vector<property> get_properties();
			void set_properties(std::vector<property>);

			property operator[](std::string name);

			// Function methods
			std::vector<function> get_functions();

			// Children methods
			std::vector<std::shared_ptr<object>> get_children();

			void add_child(std::shared_ptr<object>);

			// Request to delete an object
			bool request_destroy(bool = false);
		
			// Drawing context
			ID2D1DeviceContext* context = 0;
		protected:
			bool _destroy = false; // Destroy request state
			std::string _type; // Type of object, use type() to read/write
			std::string _id = util::generate_id(8); // Auto-generated, read-only
			std::vector<std::shared_ptr<object>> children = {}; // Children objects
			std::vector<property> properties;
			std::vector<function> functions;
			std::vector<callback> callbacks;
		};
	}
}