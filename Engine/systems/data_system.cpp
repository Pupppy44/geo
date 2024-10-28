#include "data_system.h"
#include "../core/game.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace geo {
    namespace systems {
        data_system::data_system(core::game& g) : game(&g) {
            type("data_system");

            // Initialization: Create the folder if it doesn't exist
            if (!fs::exists(data_path))
                fs::create_directory(data_path);

            define("get", [&](sol::variadic_args args) -> sol::object {
                std::string key = args[0].as<std::string>();

                // Construct file path
                std::string filePath = data_path + game->info.id + "_" + key;

                // Read data from file
                std::ifstream file(filePath);
                if (file.is_open()) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    std::string data = buffer.str();
                    file.close();
                    return sol::make_object(game->runner.lua, data);
                }

                return sol::nil;
             });

            define("set", [&](sol::variadic_args args) -> sol::object {
                std::string key = args[0].as<std::string>();
                std::string data = args[1].as<std::string>();

                // Construct file path
                std::string filePath = data_path + game->info.id + "_" + key;

                // Write data to file
                std::ofstream file(filePath);
                if (file.is_open()) {
                    file << data;
                    file.close();
                }

                return sol::nil;
            });
        }
    }
}