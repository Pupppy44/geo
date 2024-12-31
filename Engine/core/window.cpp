#include "window.h"
#include "game.h"
#include "../tree/object.h"
#include <d2d1helper.h>

namespace geo {
	namespace core {
		window::window(core::game& game)
			: game(&game),
			hwnd(NULL)
		{};

		void window::init(int width, int height, bool load_mode) {
			register_class();

			// Set dimensions
			game->info.width = width;
			game->info.height = height;

			// Position the window to the center
			int window_x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
			int window_y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

			// Set window flags based on load mode
			auto flags = WS_POPUP | WS_SYSMENU;
			if (!load_mode) {
				flags = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
			}

			// Adjust the window's rectangle
			RECT window_rect = { 0, 0, width, height };
			AdjustWindowRect(&window_rect, flags, FALSE);

			hwnd = CreateWindowW(
				L"geo_client",
				L"Geo Client",
				flags,
				window_x,
				window_y,
				window_rect.right - window_rect.left,
				window_rect.bottom - window_rect.top,
				NULL,
				NULL,
				GetModuleHandle(NULL),
				game);
		}

		void window::exit() {
			PostMessage(hwnd, WM_QUIT, 0, 0);
		}

		void window::set_title(std::string title) {
			SetWindowTextA(hwnd, title.data());
		}

		void window::set_size(int width, int height, bool borderless) {
			// Set dimensions
			game->info.width = width;
			game->info.height = height;
			
			// Position the window to the center
			int window_x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
			int window_y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

			// Set window flags based on load mode
			auto flags = WS_POPUP | WS_SYSMENU;
			if (!borderless) {
				flags = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
			}

			// Adjust the window's rectangle
			RECT window_rect = { 0, 0, width, height };
			AdjustWindowRect(&window_rect, flags, FALSE);

			SetWindowPos(hwnd, HWND_TOP, window_x, window_y, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, SWP_SHOWWINDOW);

			// Set new flags
			SetWindowLong(hwnd, GWL_STYLE, flags);
		}

		D2D1::ColorF window::get_background() {
			return background;
		}

		std::string window::get_hex_background() {
			return hex_background;
		}

		void window::set_background(std::string color) {
			hex_background = color;
			background = util::hex_to_color(color);
			game->info.background = hex_background;
		}

		void window::start() {
			if (game->info.name == "") {
				game->info.name = "Untitled Game";
			}

			// Set title
			set_title(game->info.name + " - Geo Client (Beta)");

			// Initiate all objects
			for (auto& obj : game->engine.tree.get_objects()) {
				obj->context = game->engine.get_context();
				obj->game = game;
				obj->init();
			}

			// Start all scripts
			game->runner.start();

			game->engine.reset_stop();

#ifdef _DEBUG
			game->debugger.set("game_name", game->info.name);
			game->debugger.set("game_id", game->info.id);
			game->debugger.set("game_color", get_hex_background());
			game->debugger.set("game_objects", std::to_string(game->engine.tree.get_objects().size()));
			game->debugger.set("game_scripts", game->runner.scripts);
			game->debugger.start(L"Geo Client Debugger");
#endif

			ShowWindow(hwnd, SW_SHOWNORMAL);
			UpdateWindow(hwnd);

			MSG msg = { 0 };

			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					game->engine.render();
					Sleep(1000 / 144);
				}
			}
		}

		void window::register_class() {
			WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = wndproc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = sizeof(LONG_PTR);
			wcex.hInstance = GetModuleHandle(NULL);
			wcex.hbrBackground = NULL;
			wcex.lpszMenuName = NULL;
			wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
			wcex.lpszClassName = L"geo_client";
			wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101));

			RegisterClassEx(&wcex);
		}

		LRESULT CALLBACK window::wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
			if (message == WM_CREATE) {
				LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
				core::game* game = (core::game*)pcs->lpCreateParams;

				// Set the game pointer so we can access it in the window procedure
				SetWindowLongPtrW(
					hwnd,
					GWLP_USERDATA,
					reinterpret_cast<LONG_PTR>(game)
				);

				return 1;
			}
			else {
				// Get the game pointer
				core::game* game = reinterpret_cast<core::game*>(static_cast<LONG_PTR>(
					GetWindowLongPtrW(
						hwnd,
						GWLP_USERDATA
					)));

				// Potential variables
				int mx = ((int)(short)LOWORD(lParam));
				int my = ((int)(short)HIWORD(lParam));

				if (game == nullptr) {
					return DefWindowProc(hwnd, message, wParam, lParam);
				}
				
				// Send message to all objects for object-specific handling
				for (auto& obj : game->engine.tree.get_objects()) {
					obj->message(message, wParam, lParam);
				}

				switch (message)
				{
				case WM_LBUTTONUP:
					// Check if the mouse clicked within any objects
					for (auto& obj : game->engine.tree.get_objects()) {
						float x = obj->get_property<float>("x");
						float y = obj->get_property<float>("y");
						float w = obj->get_property<float>("width");
						float h = obj->get_property<float>("height");

						if (mx > x && mx < x + w && my > y && my < y + h) {
							// Coordinates of the click as Lua objects
							std::vector<sol::object> coords = { sol::make_object(game->runner.lua, mx), sol::make_object(game->runner.lua, my) };

							// Call the object's click callback
							obj->call(
								tree::callback_type::INPUT_ON_CLICK,
								coords
							);
						}
					}
					break;
				case WM_MOUSEMOVE:
					// Check if the mouse is hovering over any objects
					for (auto& obj : game->engine.tree.get_objects()) {
						float x = obj->get_property<float>("x");
						float y = obj->get_property<float>("y");
						float w = obj->get_property<float>("width");
						float h = obj->get_property<float>("height");

						if (mx > x && mx < x + w && my > y && my < y + h) {
							// Coordinates of the hover as Lua objects
							std::vector<sol::object> coords = { sol::make_object(game->runner.lua, mx), sol::make_object(game->runner.lua, my) };

							// Call the object's hover callback
							obj->call(
								tree::callback_type::INPUT_ON_HOVER,
								coords
							);
						}
					}
					break;
				case WM_KEYDOWN:
					// Call key down callbacks
					game->runner.callbacks.call_input(
						tree::callback_type::INPUT_ON_KEY_DOWN,
						util::vk_to_string(static_cast<int>(wParam))
					);
					break;
				case WM_KEYUP:
					// Call key up callbacks
					game->runner.callbacks.call_input(
						tree::callback_type::INPUT_ON_KEY_UP,
						util::vk_to_string(static_cast<int>(wParam))
					);
					break;
				case WM_CLOSE:
					DestroyWindow(hwnd);
					break;
				case WM_DESTROY:
					PostQuitMessage(0);
					break;
				default:
					return DefWindowProc(hwnd, message, wParam, lParam);
				}

				return 0;
			}
		}
	}
}