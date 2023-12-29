#pragma once
#include <string>
#define _WINSOCKAPI_ 
#include <windows.h>
#include <d2d1_3.h>

namespace geo {
	namespace core {
		class game;

		class window {
		public:
			window(game&);

			// Initiate the window
			void init(int, int, bool);

			// Clear the window
			void clear();

			// Exit the window
			void exit();

			// Get the window handle
			HWND get_hwnd() {
				return hwnd;
			}

			// Get the window's background
			D2D1::ColorF get_background();
			std::string get_hex_background();

			// Set the window background
			void set_background(std::string);

			// Set the window title
			void set_title(std::string);
			
			// Show the window
			void start();
		private:
			// Register the window class
			void register_class();

			// Window procedure
			static LRESULT CALLBACK wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		private:
			game* game;
			HWND hwnd;
			
			// Backgrounds as RGB and hexadecimal
			D2D1::ColorF background = D2D1::ColorF(0, 0, 0, 1);
			std::string hex_background = "#000000";
		};
	}
}
