#pragma once
#include <iostream>
#include <map>
#include <string>
#include <thread>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Richedit.h>

namespace geo {
	namespace util {
        class debugger {
        public:
            debugger() : hwnd(nullptr), edit_hwnd(nullptr) {}

            // Starts the debugger UI
            void start(LPCWSTR title = L"Debugger", int width = 400, int height = 300) {
                std::jthread message_thread([&, title, width, height]() {
                    // Register the window class
                    WNDCLASS wc = { 0 };
                    wc.lpfnWndProc = WindowProc;
                    wc.hInstance = GetModuleHandle(nullptr);
                    wc.lpszClassName = L"geo_debugger_class";
                    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101));
                    RegisterClassW(&wc);

                    // Create the window
                    hwnd = CreateWindowExW(0, L"geo_debugger_class", title, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

                    if (hwnd == nullptr) {
                        std::cerr << "(debugger) failed to create window" << std::endl;
                        return;
                    }

                    // Create the text label with white background and custom font
                    edit_hwnd = CreateWindowExA(0, "EDIT", nullptr, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                        0, 0, width, height, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

                    if (edit_hwnd == nullptr) {
                        std::cerr << "(debugger) failed to create text label" << std::endl;
                        DestroyWindow(hwnd);
                        hwnd = nullptr;
                        return;
                    }

                    // Create the text font
                    HFONT hFont = CreateFont(
                        16,                    // Height of the font
                        0,                     // Width of the font
                        0,                     // Angle of escapement
                        0,                     // Orientation angle
                        FW_BOLD,             // Font weight
                        FALSE,                 // Italic
                        FALSE,                 // Underline
                        0,                     // Strikeout
                        ANSI_CHARSET,          // Character set identifier
                        OUT_DEFAULT_PRECIS,    // Output precision
                        CLIP_DEFAULT_PRECIS,   // Clipping precision
                        DEFAULT_QUALITY,       // Output quality
                        DEFAULT_PITCH | FF_SWISS, // Pitch and family
                        L"Fira Mono"               // Font name
                    );

                    if (hFont) {
                        SendMessage(edit_hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
                    }

                    // Show the window
                    ShowWindow(hwnd, SW_SHOWDEFAULT);
                    UpdateWindow(hwnd);

                    // Enter the message loop
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
                            // Update the UI constantly
                            update_ui();
                            Sleep(1000 / 60);
                        }
                    }
                    });

                message_thread.detach();
            }


            // Ends the debugger UI
            void end() {
                if (hwnd != nullptr) {
                    DestroyWindow(hwnd);
                    hwnd = nullptr;
                }
            }

            // Sets a value
            void set(const std::string& name, const std::string& value) {
#ifdef _DEBUG
                values[name] = value;
                update_ui();
#endif
            }

            void set(const std::string& name, int value) {
                set(name, std::to_string(value));
            }

            void set(const std::string& name, bool value) {
                set(name, std::string(value ? "true" : "false"));
            }

            // Removes a value
            void remove(const std::string& name) {
#ifdef _DEBUG
                values.erase(name);
                update_ui();
#endif
            }
        private:
            // Procedure for the debugger window
            static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
                switch (uMsg) {
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
                case WM_CREATE:
                    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
                    return 0;
                }
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }

            // Update the UI with the current debug values
            void update_ui() {
                std::string content;

                for (const auto& pair : values) {
                    content += pair.first + ": " + pair.second + "\r\n";
                }

                SetWindowTextA(edit_hwnd, content.c_str());
            }

            HWND hwnd;
            HWND edit_hwnd;
            std::map<std::string, std::string> values; // Debug values
        };
	}
}

