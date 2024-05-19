#include "window.h"
#include "game.h"

namespace geo {
	namespace core {
		window::window(core::game& game)
			: game(&game),
			hwnd(NULL)
		{};

		void window::init(int width, int height, bool load_mode) {
			register_class();

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

		void window::clear() {
			HDC hdc = GetDC(hwnd);
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);

			// Clears the game window
			PatBlt(hdc, clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, WHITENESS);

			ReleaseDC(hwnd, hdc);
		}

		void window::exit() {
			PostMessage(hwnd, WM_QUIT, 0, 0);
		}

		void window::set_title(std::string title) {
			SetWindowTextA(hwnd, title.data());
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
		}

		void window::start() {
			if (game->name == "") {
				game->name = "Untitled Game";
			}

			// Set title
			set_title(game->name + " - Geo Client (Beta)");

			// Initiate all objects
			for (auto& obj : game->engine.tree.get_objects()) {
				obj->context = game->engine.get_context();
				obj->init();
			}

			// Start all scripts
			game->runner.start();

#ifdef _DEBUG
			game->debugger.set("game_name", game->name);
			game->debugger.set("game_id", game->id);
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
					Sleep(1000 / 60);
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

				switch (message)
				{
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