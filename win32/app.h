#pragma once
#include <Windows.h>
#include <functional>
#include <string>

namespace win32 {
    // represents a minimal win32 app
    class app {
    public:
        app(const std::string& class_name = "AppMsgReceiver", const std::string& window_title = "Receiver");
        ~app();

        HWND get_hwnd() { return hwnd; }

        // https://learn.microsoft.com/en-gb/windows/win32/api/winuser/nf-winuser-addclipboardformatlistener?redirectedfrom=MSDN
        void add_clipboard_listener();

        // runs message loop until WM_QUIT
        void run() const;

        // called when an unhandled message arrives to this class' fake window
        std::function<LRESULT(UINT, WPARAM, LPARAM)> on_app_message;

        // called from message loop (run()) when any message arrives
        std::function<void(MSG&)> on_any_message;

        void set_message_timeout(size_t milliseconds = -1);

    private:
        WNDCLASSEX wc;
        HWND hwnd{ nullptr };
        UINT_PTR timeout_timer_id{0};

        static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };
}