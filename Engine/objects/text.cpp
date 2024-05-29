#include "text.h"
#pragma comment(lib, "dwrite.lib")

namespace geo {
	namespace objects {
		text::text() {
			type("text");
		};

		void text::init() {
			DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(write_factory),
				reinterpret_cast<IUnknown**>(&write_factory)
			);

			if (font_collection == NULL) {
				write_factory->GetSystemFontCollection(&font_collection);
			}

			// Create the text format
			write_factory->CreateTextFormat(
				util::string_to_wchar(get_property<std::string>("font")),
				font_collection,
				(DWRITE_FONT_WEIGHT)get_property<float>("weight"),
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_SEMI_EXPANDED,
				get_property<float>("size") * (4 / 3), // Pixels to points
				L"",
				&text_format
			);

			// Create the text's color brush
			auto color = util::hex_to_color(get_property<std::string>("color"));
			context->CreateSolidColorBrush(
				color,
				&brush
			);
		};

		void text::render() {
			// Text properties
			auto text = util::string_to_wchar(get_property<std::string>("text"));
			auto x = get_property<float>("x");
			auto y = get_property<float>("y");
			auto w = get_property<float>("width");
			auto h = get_property<float>("height");
			auto align = get_property<std::string>("align");

			// Set the text alignment
			if (align == "center") {
				text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
			else if (align == "right") {
				text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
				text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
			else {
				text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}

			// Nobody wants aliased text...right?
			context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
			
			
			// Draw text
			context->DrawText(
				text,
				static_cast<UINT32>(wcslen(text) + 1),
				text_format,
				D2D1::RectF(x, y, w + x, h + y),
				brush
			);
		};
	}
}