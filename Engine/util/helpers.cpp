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
			if (hex == "" || hex.length() < 7) {
				return D2D1::ColorF(0, 0, 0, 1);
			}
			
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
			case VK_LEFT:
				key_string = "Left Arrow";
				break;
			case VK_RIGHT:
				key_string = "Right Arrow";
				break;
			case VK_UP:
				key_string = "Up Arrow";
				break;
			case VK_DOWN:
				key_string = "Down Arrow";
				break;
			default:
				// Converts to lowercase/uppercase
				// These expressions check if caps lock is on or if the shift key is held
				if (((GetKeyState(VK_CAPITAL) & 0x0001) != 0) || (GetAsyncKeyState(VK_SHIFT) < 0)) {
					// Capitalize number keys and letters
					switch (key) {
					case 0x30:
						key_string = ")";
						break;
					case 0x31:
						key_string = "!";
						break;
					case 0x32:
						key_string = "@";
						break;
					case 0x33:
						key_string = "#";
						break;
					case 0x34:
						key_string = "$";
						break;
					case 0x35:
						key_string = "%";
						break;
					case 0x36:
						key_string = "^";
						break;
					case 0x37:
						key_string = "&";
						break;
					case 0x38:
						key_string = "*";
						break;
					case 0x39:
						key_string = "(";
						break;
					default:
						// Uppercase already
						key_string = std::string(1, static_cast<char>(key));
						break;
					}
				}
				else {
					// Convert to lowercase
					key_string = std::string(1, tolower(static_cast<char>(key)));
				}

				break;
			}

			return key_string;
		}
		
		wchar_t* string_to_wchar(std::string title) {
			size_t s = strlen(title.data()) + 1;
			wchar_t* n = new wchar_t[s];
			size_t o;
			mbstowcs_s(&o, n, s, title.data(), s - 1);
			return n;
		}

		std::string download_file(std::string url) {
			char tempFilePath[MAX_PATH];
			DWORD bufferSize = MAX_PATH;

			HRESULT result = URLDownloadToCacheFileA(nullptr, url.data(), tempFilePath, bufferSize, 0, nullptr);
			if (SUCCEEDED(result)) {
				std::string path(tempFilePath);
				return path;
			}
			else {
				return "";
			}
		}

		std::string get_username() {
			char username[260 + 1];
			DWORD size = 260 + 1;
			GetUserNameA(username, &size);
			return std::string(username);
		}

		std::string get_ui(int type) {
			HMODULE hModule = GetModuleHandle(NULL);
			HRSRC hResInfo = FindResource(hModule, MAKEINTRESOURCE(type), MAKEINTRESOURCE(255));
			if (!hResInfo) return "";

			HGLOBAL hResData = LoadResource(hModule, hResInfo);
			if (!hResData) return "";

			DWORD resSize = SizeofResource(hModule, hResInfo);
			if (resSize == 0) return "";

			const char* resData = static_cast<const char*>(LockResource(hResData));
			if (!resData) return "";

			return std::string(resData, resSize);
		};
	}
}