#pragma once
#include <windows.h>
#include <wrl.h>
#include <wil/com.h>
#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

namespace studio {
	// Main studio window that loads the HTML application
    class view {
    public:
        view() : hwnd(NULL), hinst(GetModuleHandle(NULL)) {};
        
		// Initialize the window
        void init();
        
		// Show the window
        void start();
    private:
        // WebView2 components
        HWND hwnd;
        HINSTANCE hinst;
        wil::com_ptr<ICoreWebView2> webview_window;

		// Load studio.html from the resources
        LPCWSTR load_studio_html();
    };
}