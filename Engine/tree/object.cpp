#include "../core/game.h" // Includes object.h
#include <d2d1_3.h>

namespace geo {
    namespace tree {        
        // Properties
        property object::get_property(std::string name) {
            for (property& prop : properties) {
                if (prop.name == name) {
                    return prop;
                }
            }

			return property();
        }

        std::vector<property> object::get_properties() {
            return properties;
        }

        void object::set_property(property prop) {            
			// Update any existing properties
            for (auto& p : properties) {
                if (p.name == prop.name) {
                    p = prop;
                    return;
                }
            }

			properties.push_back(prop);
        }

        void object::set_properties(std::vector<property> properties) {
            for (auto& prop : properties) {
                set_property(prop);
            }
        }

        property object::operator[](std::string name) {
            return get_property(name);
        }
        
        // Functions
        void object::define(std::string name, std::function<sol::object(sol::variadic_args)> func) {
            function func_obj;
            func_obj.name = name;
            func_obj.function = func;
            functions.push_back(func_obj);
        }

        // Setup
        void object::setup() {
            // On click
            define("on_click", [=](sol::variadic_args args) {
				add_callback(INPUT_ON_CLICK, args[0].as<sol::function>());
				return sol::nil;
            });

            // On hover
            define("on_hover", [=](sol::variadic_args args) {
                add_callback(INPUT_ON_HOVER, args[0].as<sol::function>());
				return sol::nil;
            });

			// Destroy object
            define("destroy", [=](sol::variadic_args args) {
                request_destroy(true);
				return sol::nil;
            });
        }

		// Register a callback with a Lua function
		void object::add_callback(callback_type type, sol::function func) {
			// Create a callback object
			callback callback;
			callback.name = get_property<std::string>("name");
			callback.id = id();
			callback.type = type;
			callback.function = func;

			// Add the callback to the callbacks list 
			callbacks.push_back(callback);
		}

        // Call a callback
		void object::call(callback_type cb_type, std::vector<sol::object> args) {
			for (auto& callback : callbacks) {
				if (callback.type == cb_type) {
					callback.function(sol::as_args(args));
				}
			}
        }

        std::vector<function> object::get_functions() {
            return functions;
        }
        
        std::vector<std::shared_ptr<object>> object::get_children() {
			return children;
        }

		void object::add_child(std::shared_ptr<object> child) {
			children.push_back(child);
		}
        
        bool object::request_destroy(bool value) {
			if (value) {
                _destroy = true;
			}
			return _destroy;
        }
    }
}
