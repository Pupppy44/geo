#pragma once
#include <string>
#include <variant>

namespace geo {
    namespace tree {
        // Property types
        enum class property_type {
            NONE,
            NUMBER,
            STRING,
            BOOLEAN
        };

        // Property class for object properties
        class property {
        public:
            // Property name and type
            std::string name;
            property_type type;

            bool updated = false;

            property(property_type t = property_type::NONE, std::string n = "", std::string v = "") : name(n), type(t) {
                set(v);
            };

            // Get the property value
            template <typename T>
            T get() const {
                return std::get<T>(value);
            }

            // Get value as a string
            std::string as_string() {
				if (type == property_type::NUMBER)
					return std::to_string(get<float>());
				else if (type == property_type::BOOLEAN)
					return (get<bool>() ? "true" : "false");
				else
					return get<std::string>();
            }

            // Set the property value
            void set(std::string v) {
                // Convert the string value to the correct type
                if (type == property_type::NUMBER)
                    value = std::stof(v);
                else if (type == property_type::BOOLEAN)
                    value = (v == "true" ? true : false);
                else
                    value = v;
            }
        private:
            // Variant to store the property value
            std::variant<float, std::string, bool> value;
        };
    };
}
