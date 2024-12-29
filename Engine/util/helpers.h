#pragma once
#include <string>
#include <algorithm>
#include <fstream>
#pragma comment(lib, "urlmon.lib")

namespace D2D1 {
	class ColorF;
}

namespace geo {
	namespace util {
		// Generate a random string with a given length
		std::string generate_id(int);
		
		// Convert a hex color to a D2D1 ColorF
		D2D1::ColorF hex_to_color(std::string);

		// Converts a virtual key code to a string
		std::string vk_to_string(int);

		// Converts a wide char to a string
		std::string wchar_to_string(const wchar_t*);

		// Converts a string to a wide char
		wchar_t* string_to_wchar(std::string title);

		// Download a file from a URL
		std::string download_file(std::string);

		// Save a file from its Base64 representation
		std::string base64_to_file(std::string);

		// Get lastest clipboard text
		std::string get_clipboard_text();

		// Get Windows username
		std::string get_username();
		
		// Get user ID
		std::string get_user_id();

		// Get user avatar profile
		std::string get_user_avatar();

		// Get a Core UI resource as XML
		std::string get_ui(int);
	}
}
