#include "text.h"
#pragma comment(lib, "dwrite.lib")

namespace geo {
	namespace objects {
		text::text() {
			type("text");
		};

		void text::init() {
			HRESULT hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(write_factory),
				reinterpret_cast<IUnknown**>(&write_factory)
			);

			if (FAILED(hr)) {
				ERR("failed to create DirectWrite factory for text object (text id=" + id() + ")");
				return;
			}

			std::string source = get_property<std::string>("source");

			// If the font resource is a remote resource, download it and store it in a temp directory
			if (source.starts_with("http")) {
				DEBUG("downloading text font from " + get_property<std::string>("path") + " (text id=" + id() + ")");

				std::string file = util::download_file(source);

				AddFontResourceA(file.c_str());
			}

			// If the font resource is from Base64, load it
			if (source.starts_with("base64:")) {
				DEBUG("loading text font from base64 (text id=" + id() + ")");

				std::string file = util::base64_to_file(source);

				AddFontResourceA(file.c_str());
			}

			write_factory->GetSystemFontCollection(&font_collection);

			if (font_collection == nullptr) {
				ERR("failed to get system font collection for text object (text id=" + id() + ")");
				return;
			}
		};

		void text::render() {
			if (write_factory == nullptr || font_collection == nullptr) {
				return;
			}

			// Text properties
			auto text = util::string_to_wchar(get_property<std::string>("text"));
			auto x = get_property<float>("x");
			auto y = get_property<float>("y");
			auto w = get_property<float>("width");
			auto h = get_property<float>("height");
			auto size = get_property<float>("size");
			auto weight = get_property<float>("weight");
			auto font = get_property<std::string>("font");
			auto align = get_property<std::string>("align");
			auto color = util::hex_to_color(get_property<std::string>("color"));

			// Create the text format
			write_factory->CreateTextFormat(
				util::string_to_wchar(font),
				font_collection,
				(DWRITE_FONT_WEIGHT)weight,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_SEMI_EXPANDED,
				size * (4 / 3), // Pixels to points
				L"",
				&text_format
			);

			// The write factory may fail to create the text format due to missing properties
			if (text_format == nullptr) {
				WARN("failed to create text format for text object (text id=" + id() + "), cannot render text");
				return;
			}

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

			// Create the text's color brush
			context->CreateSolidColorBrush(
				color,
				&brush
			);
			
			// Draw text
			context->DrawText(
				text,
				static_cast<UINT32>(wcslen(text) + 1),
				text_format,
				D2D1::RectF(x, y, w + x, h + y),
				brush
			);

			// Release resources
			brush->Release();
			text_format->Release();
		};
	}
}