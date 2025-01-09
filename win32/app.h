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
        std::function<LRESULT(UINT, WPARAM, LPARAM)> on_app_window_message;

        // called from message loop (run()) when any message arrives
        std::function<void(MSG&)> on_message_loop_message;

        // called when keyboard hook is installed, called with the following parameters:
        // - 0. WPARAM - The identifier of one of the following messages: WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP.
        // - 1. KBDLLHOOKSTRUCT& - reference to the KBDLLHOOKSTRUCT structure, containing information about the low-level keyboard input event.
        std::function<void(WPARAM, KBDLLHOOKSTRUCT&)> on_low_level_keyboard_hook;

        void set_message_timeout(size_t milliseconds = -1);

        void set_max_fps_mode(bool v) { max_fps_mode = v; }

        bool install_low_level_keyboard_hook();

    private:
        WNDCLASSEX wc;
        HWND hwnd{ nullptr };
        UINT_PTR timeout_timer_id{0};
        bool max_fps_mode{false};
        HHOOK hLLKHook{nullptr};

        static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    };
}