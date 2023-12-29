#include "helpers.h"
#include <chrono>
#include <d2d1_3.h>
#include <iostream>

namespace geo {
	namespace util {
		std::string generate_id(int length) {
			const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

			// Precise seed
			auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			std::srand(static_cast<unsigned>(seed));

			std::string random_string(10, 0);
			std::generate_n(random_string.begin(), 10, [&charset]() { return charset[rand() % charset.size()]; });

			return random_string;
		}

		D2D1::ColorF hex_to_color(std::string hex) {
			// Remove the # from the beginning of the string
			hex.erase(0, 1);

			// Convert to a long
			long hex_long = std::stol(hex, nullptr, 16);

			// Extract the RGB values
			int r = (hex_long >> 16) & 0xFF;
			int g = (hex_long >> 8) & 0xFF;
			int b = hex_long & 0xFF;

			// Convert to a D2D1 ColorF
			return D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, 1);
		}

		std::string vk_to_string(int key) {
			std::string key_string = "";

			switch (key)
			{
			case VK_CONTROL:
				key_string = "Control";
				break;
			case VK_SHIFT:
				key_string = "Shift";
				break;
			case VK_MENU:
				key_string = "Alt";
				break;
			case VK_RETURN:
				key_string = "Enter";
				break;
			case VK_BACK:
				key_string = "Backspace";
				break;
			case VK_SPACE:
				key_string = "Spacebar";
				break;
			case VK_ESCAPE:
				key_string = "Escape";
				break;
			default:
				// Converts to lowercase/uppercase
				// These expressions check if caps lock is on or if the shift key is held
				if (((GetKeyState(VK_CAPITAL) & 0x0001) != 0) || (GetAsyncKeyState(VK_SHIFT) < 0)) {
					// Uppercase already
					key_string = std::string(1, static_cast<char>(key));
				}
				else {
					// Convert to lowercase
					key_string = std::string(1, tolower(static_cast<char>(key)));
				}

				break;
			}

			return key_string;
		}
	}
}