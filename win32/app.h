#pragma once
#include <Windows.h>
#include <functional>
#include <string>

namespace win32 {

    /**
     * @brief Represents a minimal win32 app.
     */
    class app {
    public:
        app(const std::string& class_name = "AppMsgReceiver", const std::string& window_title = "Receiver");
        ~app();

        struct mouse_hook_data {
            UINT_PTR msg;
            POINT pt;
            short wheel_delta{0};
        };

        HWND get_hwnd() { return hwnd; }

        // https://learn.microsoft.com/en-gb/windows/win32/api/winuser/nf-winuser-addclipboardformatlistener?redirectedfrom=MSDN
        
        
        /**
         * @brief WM_CLIPBOARDUPDATE is sent when the content of the clipboard changes.
         */
        void add_clipboard_listener();

        // runs message loop until WM_QUIT
        void run() const;

        // called when an unhandled message arrives to this class' fake window
        std::function<LRESULT(UINT, WPARAM, LPARAM)> on_app_window_message;

        // called from message loop (run()) when any message arrives
        std::function<void(MSG&)> on_message_loop_message;

        void set_message_timeout(size_t milliseconds = -1);

        void set_max_fps_mode(bool v) { max_fps_mode = v; }

        bool install_low_level_keyboard_hook(std::function<bool(UINT_PTR, KBDLLHOOKSTRUCT&)> fn);
        bool install_low_level_mouse_hook(std::function<bool(mouse_hook_data)> fn);
        void uninstall_low_level_keyboard_hook();
        void uninstall_low_level_mouse_hook();

        /**
         * @brief Registers a global hotkey for this app. When pressed, it will send WM_HOTKEY message to this app's message queue. See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-hotkey on how to handle it. For full example see https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey#examples
         * @param hotkey_id Unique ID for this hotkey. You can use it to identify which hotkey was pressed.
         * @param vk_code Virtual key code. See https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
         * @param modifiers Modifiers. See https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
         * @return empty string on success, or error message
         */
        std::string register_global_hotkey(int hotkey_id, unsigned int vk_code, unsigned int modifiers = 0);

        void unregister_global_hotkey(int hotkey_id);

        void unregister_all_global_hotkeys();

        /**
         * @brief Helper function to check if a message is a hotkey message for a specific hotkey ID.
         * @param msg 
         * @param wParam 
         * @param hotkey_id 
         * @return 
         */
        bool is_hotkey_message(UINT msg, WPARAM wParam, int hotkey_id) const;

    private:
        WNDCLASSEX wc;
        HWND hwnd{ nullptr };
        UINT_PTR timeout_timer_id{0};
        bool max_fps_mode{false};
        HHOOK hLLKbdHook{nullptr};
        HHOOK hLLMouseHook{nullptr};
        std::vector<int> registered_hotkeys;  // to keep track of registered hotkeys, so we can unregister them on exit

        static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

        std::function<bool(UINT_PTR, KBDLLHOOKSTRUCT&)> on_low_level_keyboard_hook_func;
        std::function<bool(mouse_hook_data)> on_low_level_mouse_hook_func;
    };
}