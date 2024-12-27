#pragma once
#include <string>
#include <vector>
#include <regex>
#include <d2d1_3.h>
#include <dwrite.h>
#include "helpers.h"

namespace geo {
	namespace util {
		// Class for parsing and rendering rich text for the text object
		class rich_text {
		public:
			enum class rich_text_type {
				none,
				bold,
				italics,
				underline,
				strikethrough,
				color
			};
			
			struct rich_text_element {
				rich_text_type type;
				DWRITE_TEXT_RANGE range;
				std::string value; // For color

				// Apply the text format to the text
				void apply(ID2D1DeviceContext* context, IDWriteTextLayout* layout) {
					switch (type) {
					case rich_text_type::bold:
						layout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, range);
						break;
					case rich_text_type::italics:
						layout->SetFontStyle(DWRITE_FONT_STYLE_ITALIC, range);
						break;
					case rich_text_type::underline:
						layout->SetUnderline(true, range);
						break;
					case rich_text_type::strikethrough:
						layout->SetStrikethrough(true, range);
						break;
					case rich_text_type::color:
						ID2D1SolidColorBrush* brush = {};
						context->CreateSolidColorBrush(util::hex_to_color(value), &brush);
						layout->SetDrawingEffect(brush, range);
						break;
					}
				}
			};

			static std::vector<rich_text_element> parse(std::string text) {
				std::vector<rich_text_element> elements;
				std::smatch match;
				// <b />, <i />, <u />, <s />, <color=#ffffff />
				std::regex re(R"(<(b|i|u|s|color=[#0-9a-fA-F]{7})>(.*?)<\/\1>|<\/color>)");

				auto it = std::sregex_iterator(text.begin(), text.end(), re);
				auto end = std::sregex_iterator();

				std::string stripped_text = text; 
				
				for (; it != end; ++it) {
					std::smatch match = *it;
					std::string tag = match[1].str();
					std::string content = match[2].str();
					DWRITE_TEXT_RANGE range;
					range.startPosition = match.position(2);
					range.length = content.length();

					if (tag == "b") {
						elements.push_back({ rich_text_type::bold, range, "" });
					}
					else if (tag == "i") {
						elements.push_back({ rich_text_type::italics, range, "" });
					}
					else if (tag == "u") {
						elements.push_back({ rich_text_type::underline, range, "" });
					}
					else if (tag == "s") {
						elements.push_back({ rich_text_type::strikethrough, range, "" });
					}
					else if (tag.rfind("color", 0) == 0) {
						elements.push_back({ rich_text_type::color, range, tag.substr(6) });
					}
				}

				return elements;
			}
		};
	}
}