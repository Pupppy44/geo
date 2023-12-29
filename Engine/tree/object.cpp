#include "object.h"
#include "../server/server.h"
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
        
        std::vector<function> object::get_functions() {
            return functions;
        }
    }
}
