#pragma once
#include <string>
#include <algorithm>

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
	}
}
