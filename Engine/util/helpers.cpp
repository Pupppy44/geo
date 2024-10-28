#include "helpers.h"
#include <chrono>
#include <d2d1_3.h>
#include <iostream>
#include <windows.h>
#include <sddl.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

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
			// Strict check
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

		std::string wchar_to_string(const wchar_t* title) {
			size_t s = wcslen(title) + 1;
			char* n = new char[s];
			size_t o;
			wcstombs_s(&o, n, s, title, s - 1);
			std::string str(n);
			delete[] n;
			return str;
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

			// Download the file to tempFilePath
			HRESULT result = URLDownloadToCacheFileA(nullptr, url.data(), tempFilePath, bufferSize, 0, nullptr);
			if (SUCCEEDED(result)) {
				std::string path(tempFilePath);
				return path;
			}
			else {
				return "";
			}
		}

		std::string base64_to_file(std::string base64) {
			// If the base64 string starts with "base64:" then remove it
			if (base64.find("base64:") == 0) {
				base64 = base64.substr(7);
			}
			
			// Base64 to binary file
			std::string decoded_data;
			std::vector<int> T(256, -1);
			for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
			int val = 0, valb = -8;
			for (unsigned char c : base64) {
				if (T[c] == -1) break;
				val = (val << 6) + T[c];
				valb += 6;
				if (valb >= 0) {
					decoded_data.push_back(char((val >> valb) & 0xFF));
					valb -= 8;
				}
			}

			// Get temp file path
			char temp_path[MAX_PATH];
			GetTempPathA(MAX_PATH, temp_path);
			std::string temp_file = std::string(temp_path) + util::generate_id(8);

			// Write decoded data to file
			std::ofstream outfile(temp_file, std::ofstream::binary);
			if (outfile) {
				outfile.write(decoded_data.c_str(), decoded_data.length());
				outfile.close();
				return temp_file;
			}

			return "";
		}

		std::string get_username() {
			char username[260 + 1];
			DWORD size = 260 + 1;
			// Find the username of the current user
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

			// Get the string from the resource
			const char* resData = static_cast<const char*>(LockResource(hResData));
			if (!resData) return "";

			return std::string(resData, resSize);
		};

		std::string get_user_id() {
			const std::string username = get_username();
			
			DWORD sidSize = SECURITY_MAX_SID_SIZE;
			PSID securityIdentifier = (PSID)malloc(sidSize);
			char referencedDomainName[256];
			DWORD cchReferencedDomainName = 256;
			SID_NAME_USE sidType;

			// Fetch the SID of the logged in user
			if (LookupAccountNameA(NULL, username.c_str(), securityIdentifier, &sidSize, referencedDomainName, &cchReferencedDomainName, &sidType)) {
				LPSTR sidString;
				ConvertSidToStringSidA(securityIdentifier, &sidString);
				std::string result(sidString);
				LocalFree(sidString);
				free(securityIdentifier);
				return result;
			}

			free(securityIdentifier);
			
			return "-1";
		}

		std::string get_user_avatar() {
			// Get user id (security ID on Windows)
			std::string result = get_user_id();
			
			if (result == "-1") {
				return "";
			}
				
			// Profile pictures are stored in AccountPictures\{user_id}
			std::string accountPicturesPath = "C:\\Users\\Public\\AccountPictures\\" + result;
			std::vector<std::filesystem::path> jpgFiles;

			for (const auto& entry : std::filesystem::directory_iterator(accountPicturesPath)) {
				if (entry.is_regular_file() && entry.path().extension() == ".jpg") {
					jpgFiles.push_back(entry.path());
				}
			}

			// This sorting should get us the highest quality image.
			// The highest quality image should be the last one based on the file name
			std::sort(jpgFiles.begin(), jpgFiles.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) {
				return a.filename().string() < b.filename().string();
			});

			// Return the found profile picture path
			if (!jpgFiles.empty()) {
				return jpgFiles.front().string();
			}

			return "";
		}
	}
}