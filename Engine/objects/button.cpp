#include "button.h"

namespace geo {
	namespace objects {
		button::button() {
			type("button");
		}

		void button::init() {
			// Initialize any button-specific data or resources here
			text = L"Click Me"; // Default text on the button

			// Assuming you have a DWriteContext available in your D2D1Context
			write_context = context->GetWriteContext();
		}

		void button::render() {
			// Assuming you have a position and size for your button, adjust accordingly
			float buttonWidth = 100.0f;
			float buttonHeight = 40.0f;
			float cornerRadius = 5.0f; // Adjust based on your preference

			// Set up a rounded rectangle for the button
			D2D1_ROUNDED_RECT roundedRect = { position.x, position.y, position.x + buttonWidth, position.y + buttonHeight, cornerRadius, cornerRadius };

			// Set the button color (replace with your desired color)
			D2D1_COLOR_F buttonColor = D2D1::ColorF(1.0f, 0.0f, 0.0f); // Red color as an example

			// Fill the rounded rectangle with the specified color
			context->FillRoundedRectangle(roundedRect, context->GetSolidColorBrush(buttonColor));

			// Set up text format (replace with your desired font and size)
			write_context->CreateTextFormat(L"Arial", 14.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, textFormat.GetAddressOf());

			// Set the text color (replace with your desired color)
			D2D1_COLOR_F textColor = D2D1::ColorF(1.0f, 1.0f, 1.0f); // White color as an example

			// Create a layout to hold the text
			write_context->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.length()), textFormat.Get(), buttonWidth, buttonHeight, textLayout.GetAddressOf());

			// Set the position for the text within the button
			D2D1_POINT_2F textPosition = { position.x + 10.0f, position.y + 10.0f }; // Adjust based on your preference

			// Draw the text on the button
			write_context->DrawTextLayout(textPosition, textLayout.Get(), context->GetSolidColorBrush(textColor));

			// You might want to add additional rendering logic for the button

			// Note: Ensure you have appropriate error handling and resource management in a complete application
		}
	}
}