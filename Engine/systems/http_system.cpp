#include "../core/game.h"
#include "http_system.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <nlohmann/json.hpp>

namespace geo {
	namespace systems {
		http_system::http_system(core::game& g) : game(&g) {
			type("http_system");

			define("get", [&](sol::variadic_args args) -> sol::object {
				std::string url = args[0].as<std::string>();
				sol::function callback = args[1].as<sol::function>();

				// Use a thread so it doesn't block the code
				std::jthread([=]() {
					try {
						callback(get(url).to_table(game->runner.lua));
					}
					catch (...) {
						callback(sol::nil);
					}
				}).detach();


				return sol::nil;
			});

			define("json_parse", [&](sol::variadic_args args) -> sol::object {
				std::string json_str = args[0].as<std::string>();

				return sol::make_object(game->runner.lua, json_parse(json_str));
			});

			define("json_stringify", [&](sol::variadic_args args) -> sol::object {
				sol::table table = args[0].as<sol::table>();

				return sol::make_object(game->runner.lua, json_stringify(table));
				});
		}

		http_system::http_response http_system::get(std::string url) {
			http_response res;

			winrt::Windows::Foundation::Uri uri{ winrt::to_hstring(url) };
			winrt::Windows::Web::Http::HttpClient http_client{};

			try {
				// Send the request
				auto response = http_client.GetStringAsync(uri);
				std::string data = winrt::to_string(response.get());

				res.status = 200;
				res.data = data;

				return res;
			}
			catch (winrt::hresult_error const& ex) {
				// Something went wrong!
				res.status = ex.code();
				res.data = winrt::to_string(ex.message());
			}
		}

		sol::table http_system::json_parse(std::string json_str) {
			nlohmann::json json = nlohmann::json::parse(json_str);

			sol::table result = game->runner.lua.create_table();

			// Iterate over the JSON object and add key-value pairs to the Lua table
			for (auto& entry : json.items()) {
				// Add based on type
				if (entry.value().is_number()) {
					result[entry.key()] = entry.value().get<float>();
				}
				else if (entry.value().is_boolean()) {
					result[entry.key()] = entry.value().get<bool>();
				}
				else if (entry.value().is_string()) {
					result[entry.key()] = entry.value().get<std::string>();
				}
				else if (entry.value().is_null()) {
					result[entry.key()] = sol::nil;
				}
				else if (entry.value().is_object()) {
					// Another object, so use the same function to parse it
					result[entry.key()] = json_parse(entry.value().dump());
				}
			}

			return result;
		}

		std::string http_system::json_stringify(sol::table table) {
			nlohmann::json json;

			// Iterate over the Lua table and add key-value pairs to the JSON object
			for (auto& entry : table) {
				auto name = entry.first.as<std::string>();
				auto& value = entry.second;

				// Add based on type
				if (value.get_type() == sol::type::string) {
					json[name] = entry.second.as<std::string>();
				}
				else if (value.get_type() == sol::type::number) {
					json[name] = entry.second.as<float>();
				}
				else if (value.get_type() == sol::type::boolean) {
					json[name] = entry.second.as<bool>();
				}
				else if (value.get_type() == sol::type::nil) {
					json[name] = NULL;
				}
				else if (value.get_type() == sol::type::table) {
					// Parse this Lua table by stringifying it and parsing again
					json[name] = nlohmann::json::parse(json_stringify(value));
				}
			}

			return json.dump();
		}
	}
}