#include "view.h"
#include "../resource.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace Microsoft::WRL;

LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM);
static wil::com_ptr<ICoreWebView2Controller> webview_controller;

LPCWSTR studio::view::load_studio_html() {
    HMODULE hModule = GetModuleHandle(NULL);
    HRSRC hResInfo = FindResource(hModule, MAKEINTRESOURCE(STUDIO_HTML), MAKEINTRESOURCE(RT_HTML));
    if (!hResInfo) return L"";

    HGLOBAL hResData = LoadResource(hModule, hResInfo);
    if (!hResData) return L"";

    DWORD resSize = SizeofResource(hModule, hResInfo);
    if (resSize == 0) return L"";

    const char* resData = static_cast<const char*>(LockResource(hResData));
    if (!resData) return L"";

    std::string html(resData, resSize);
    std::string tempPath = "C:\\Windows\\Temp\\geostudio.html";

    std::ofstream file(tempPath);
    file << html;
    file.close();

    return L"C:\\Windows\\Temp\\geostudio.html";
}

void studio::view::init() {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wnd_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hinst;
    wcex.hIcon = LoadIcon(hinst, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"geostudio";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) throw std::runtime_error("Window class registration failed");

	// Center the window on the screen using default window size
    int x = (GetSystemMetrics(SM_CXSCREEN) - 1492) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - 794) / 2;

    hwnd = CreateWindow(
        L"geostudio",
        L"Geo Studio",
		// Certain styles to achieve a custom window frame
        WS_POPUP | WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME,
        x, y, 1492, 794,
        NULL, NULL, hinst, NULL
    );

    if (!hwnd) throw std::runtime_error("Window creation failed");

	// Set options for draggable regions and autoplay policy
    auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
    options->put_AdditionalBrowserArguments(L"--autoplay-policy=no-user-gesture-required --enable-features=msWebView2EnableDraggableRegions");

	// Create WebView2 environment and controller
    CreateCoreWebView2EnvironmentWithOptions(nullptr, L"C:\\Windows\\Temp", options.Get(),
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                env->CreateCoreWebView2Controller(hwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    [this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                        if (controller != nullptr) {
                            webview_controller = controller;
                            webview_controller->get_CoreWebView2(&webview_window);
                        }

#ifndef _DEBUG
                        // Disable the context menu if release build
                        ICoreWebView2Settings* settings;
                        webviewWindow->get_Settings(&settings);
                        settings->put_AreDefaultContextMenusEnabled(FALSE);
                        settings->put_AreDevToolsEnabled(FALSE);
                        settings->put_IsStatusBarEnabled(FALSE);
#endif

                        RECT bounds;
                        GetClientRect(hwnd, &bounds);
                        webview_controller->put_Bounds(bounds);

						// Loads the single HTML file that contains the entire application
						// The HTML is saved as temporary file for React Router to work correctly
                        LPCWSTR studioPath = load_studio_html();
                        webview_window->Navigate(studioPath);

                        return S_OK;
                    }).Get());
                return S_OK;
            }).Get());
}

void studio::view::start() {
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
        // ! Hides the one-pixel top border of the window
    case WM_NCCALCSIZE:
    {
        if (wParam == TRUE)
        {
            WINDOWPLACEMENT wp;
            wp.length = sizeof(WINDOWPLACEMENT);
            GetWindowPlacement(hWnd, &wp);

            // Only adjust the top border if the window is not maximized or minimized
            if (wp.showCmd != SW_SHOWMAXIMIZED && wp.showCmd != SW_SHOWMINIMIZED)
            {
                NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lParam;
                params->rgrc[0].top += 1;
            }
        }

        return 0;
        break;
    }
    // ! Prevents the window from covering the taskbar when maximized
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;
        RECT rect;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
        minMaxInfo->ptMaxPosition.x = rect.left;
        minMaxInfo->ptMaxPosition.y = rect.top;
        minMaxInfo->ptMaxSize.x = rect.right - rect.left;
        minMaxInfo->ptMaxSize.y = rect.bottom - rect.top;
        break;
    }
    // ! Resizes the window when resized by the user
    case WM_SIZE:
        if (webview_controller != nullptr) {
            RECT bounds;
            GetClientRect(hWnd, &bounds);
            webview_controller->put_Bounds(bounds);
        };
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}