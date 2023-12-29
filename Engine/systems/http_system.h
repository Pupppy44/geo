#pragma once
#include "../tree/object.h"

namespace sol {
	class state;
}

namespace geo {
	namespace core {
		class game;
	}

	namespace systems {
		// System for sending HTTP requests and encoding/decoding JSON
		class http_system : public tree::object {
		public:
			// HTTP response structure
			struct http_response {
				int status;
				std::string data;

				sol::table to_table(sol::state& lua) const {
					sol::table table = lua.create_table();
					table["status"] = status;
					table["data"] = data;
					return table;
				}
			};
			
			http_system(core::game&);

			void init() {};
			void render() {};

			// HTTP functions
			http_response get(std::string url);

			// JSON functions
			sol::table json_parse(std::string);
			std::string json_stringify(sol::table);
		private:
			core::game* game;
		};
	}
}

