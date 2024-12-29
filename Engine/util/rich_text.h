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

			static std::pair<std::string, std::vector<rich_text_element>> parse(std::string text) {
				std::vector<rich_text_element> elements;
				std::string cleaned_text;
				std::regex re(R"(<(b|i|u|s|color=[#0-9a-fA-F]{7})>(.*?)<\/(b|i|u|s|color)>)");
				std::smatch match;

				size_t last_pos = 0;

				// Iterate through matches
				auto it = std::sregex_iterator(text.begin(), text.end(), re);
				auto end = std::sregex_iterator();

				for (; it != end; ++it) {
					match = *it;

					// Add unmatched text before the match to the cleaned string
					if (match.position() > last_pos) {
						cleaned_text.append(text.substr(last_pos, match.position() - last_pos));
					}

					// Extract tag and content
					std::string tag = match[1].str();
					std::string content = match[2].str();

					// Append content (without tags) to the cleaned string
					size_t content_start = cleaned_text.size();
					cleaned_text.append(content);

					// Create the rich text element for the matched tag
					DWRITE_TEXT_RANGE range = {
						static_cast<UINT32>(content_start),       // Start position in the cleaned string
						static_cast<UINT32>(content.size())       // Length of the content
					};

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

					// Update last position
					last_pos = match.position() + match.length();
				}

				// Append any remaining text after the last match
				if (last_pos < text.size()) {
					cleaned_text.append(text.substr(last_pos));
				}

				return { cleaned_text, elements };
			}
		};
	}
}