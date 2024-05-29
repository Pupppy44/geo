#include "object.h"
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
			// Easy lambda to define a callback
			auto define_callback = [this](callback_type type, sol::variadic_args args) -> sol::object {
                // Create a callback object
                callback callback;
                callback.name = get_property<std::string>("name");
                callback.id = id();
                callback.type = type;
                callback.function = args[0].as<sol::function>();

                // Add the callback to the callbacks list 
                callbacks.push_back(callback);

				return sol::nil;
			};
            
            // On click
            define("on_click", [=](sol::variadic_args args) {
				return define_callback(INPUT_ON_CLICK, args);
            });

            // On hover
            define("on_hover", [=](sol::variadic_args args) {
                return define_callback(INPUT_ON_HOVER, args);
            });
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
    }
}
