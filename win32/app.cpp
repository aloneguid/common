#include "app.h"
#include "kernel.h"
#include "../str.h"

using namespace std;

// Define a unique identifier for the hotkey
#define HOTKEY_ONE_ID 1

namespace win32 {

    app* low_level_kbd_hook_app{nullptr};
    app* low_level_mouse_hook_app{nullptr};

    void app::set_message_timeout(size_t milliseconds) {
        if(milliseconds > 0) {
            timeout_timer_id = ::SetTimer(NULL, NULL, milliseconds, NULL);
        } else if(timeout_timer_id != 0) {
            ::KillTimer(NULL, timeout_timer_id);
        }
    }

    bool app::install_low_level_keyboard_hook(std::function<bool(UINT_PTR, KBDLLHOOKSTRUCT&)> fn) {

        uninstall_low_level_keyboard_hook();

        on_low_level_keyboard_hook_func = nullptr;
        hLLKbdHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

        if(hLLKbdHook) {
            low_level_kbd_hook_app = this;
            on_low_level_keyboard_hook_func = fn;
        } else {
            // error
            string error = kernel::get_last_error_text();
        }

        return hLLKbdHook != NULL;
    }

    bool app::install_low_level_mouse_hook(std::function<bool(mouse_hook_data)> fn) {
        uninstall_low_level_mouse_hook();

        on_low_level_mouse_hook_func = nullptr;
        hLLMouseHook = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
        if(hLLMouseHook) {
            low_level_mouse_hook_app = this;
            on_low_level_mouse_hook_func = fn;
        } else {
            // error
            string error = kernel::get_last_error_text();
        }
        return hLLMouseHook != NULL;
    }

    void app::uninstall_low_level_keyboard_hook() {
        // uninstall keyboard hook
        if(hLLKbdHook) {
            ::UnhookWindowsHookEx(hLLKbdHook);
        }
        low_level_kbd_hook_app = nullptr;
    }

    void app::uninstall_low_level_mouse_hook() {
        // uninstall mouse hook
        if(hLLMouseHook) {
            ::UnhookWindowsHookEx(hLLMouseHook);
        }
        low_level_mouse_hook_app = nullptr;
    }

    string app::register_global_hotkey(int hotkey_id, unsigned int vk_code, unsigned int modifiers) {
        BOOL ok = ::RegisterHotKey(hwnd,
            hotkey_id, 
            modifiers, vk_code);

        if(ok) {
            // check if the hotkey is already in the list
            if(std::find(registered_hotkeys.begin(), registered_hotkeys.end(), hotkey_id) == registered_hotkeys.end()) {
                // add to the list
                registered_hotkeys.push_back(hotkey_id);
            }

        }

        return kernel::get_last_error_text();
    }

    void app::unregister_global_hotkey(int hotkey_id) {
        // unregister the hotkey
        if(::UnregisterHotKey(hwnd, hotkey_id)) {
            // remove from the list
            registered_hotkeys.erase(std::remove(registered_hotkeys.begin(), registered_hotkeys.end(), hotkey_id), registered_hotkeys.end());
        }
    }

    void app::unregister_all_global_hotkeys() {
        for(int hotkey_id : registered_hotkeys) {
            ::UnregisterHotKey(hwnd, hotkey_id);
        }
        registered_hotkeys.clear();
    }

    bool app::is_hotkey_message(UINT msg, WPARAM wParam, int hotkey_id) const {
        return msg == WM_HOTKEY && wParam == hotkey_id;
    }

    LRESULT WINAPI win32::app::WndProc(
        HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam) {

        // get "this"
        app* app_this = (app*)::GetProp(hWnd, L"this");
        if (app_this && app_this->on_app_window_message) {
            LRESULT res = app_this->on_app_window_message(msg, wParam, lParam);

            return res == 0 ? ::DefWindowProc(hWnd, msg, wParam, lParam) : res;
        }

        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

    // https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc
    LRESULT app::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if(nCode >= 0) {
            if(low_level_kbd_hook_app) {
                if(low_level_kbd_hook_app->on_low_level_keyboard_hook_func) {
                    KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
                    if(low_level_kbd_hook_app->on_low_level_keyboard_hook_func(wParam, *p)) {
                        return ::CallNextHookEx(NULL, nCode, wParam, lParam);
                    } else {
                        return 1; // block the event
                    }
                }
            }
        }
        return ::CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    // https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelmouseproc
    LRESULT app::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if(nCode == HC_ACTION) {
            if(low_level_mouse_hook_app) {
                if(low_level_mouse_hook_app->on_low_level_mouse_hook_func) {
                    MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;

                    //bool is_injected = p->flags & LLMHF_INJECTED;
                    
                    mouse_hook_data mhd{wParam, p->pt, 0};

                    if(wParam == WM_MOUSEWHEEL) {
                        // the high-order word of p->mouseData member is the wheel delta.
                        // The low-order word is reserved.
                        // A positive value indicates that the wheel was rotated forward, away from the user; a negative value indicates that the wheel was rotated backward, toward the user. One wheel click is defined as WHEEL_DELTA, which is 120.

                        mhd.wheel_delta = (short)(HIWORD(p->mouseData));
                        mhd.wheel_delta /= WHEEL_DELTA;
                    }

                    if(low_level_mouse_hook_app->on_low_level_mouse_hook_func(mhd)) {
                        return ::CallNextHookEx(NULL, nCode, wParam, lParam);
                    } else {
                        return 1; // block the event
                    }
                }
            }
        }
        return ::CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    app::app(const string& class_name, const string& window_title) {

        wstring w_class_name = str::to_wstr(class_name);

        wc = {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            WndProc,
            0L,
            0L,
            GetModuleHandle(nullptr),
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            w_class_name.c_str(),
            nullptr
        };

        if (!::RegisterClassEx(&wc))
            return;

        DWORD dwStyle = WS_OVERLAPPEDWINDOW;
        DWORD dwExStyle = 0;
        hwnd = ::CreateWindowEx(dwExStyle,
            wc.lpszClassName,
            str::to_wstr(window_title).c_str(),
            dwStyle,
            100, 100, 100, 50,
            nullptr, NULL, wc.hInstance,

            // pass "this" pointer to WM_CREATE. Unfortunately it's not available after WM_CREATE has fired.
            this);
        if (!hwnd) return;

        // pass "this" as window prop to retreive it later, because WndProc is a static function
        ::SetProp(hwnd, L"this", this);

        ::ShowWindow(hwnd, SW_HIDE);
    }

    app::~app() {
        if(timeout_timer_id != 0) {
            ::KillTimer(NULL, timeout_timer_id);
        }
        ::PostQuitMessage(0);
        ::DestroyWindow(hwnd);
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);

        uninstall_low_level_keyboard_hook();
        uninstall_low_level_mouse_hook();
        unregister_all_global_hotkeys();
    }

    void app::add_clipboard_listener() {
        ::AddClipboardFormatListener(hwnd);
	}

    void app::run() const {
        MSG msg;
        while (::GetMessage(&msg, nullptr, 0, 0)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);

            if(on_message_loop_message) {
                on_message_loop_message(msg);
            }

            while(max_fps_mode) {
                while(::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);

                    if(on_message_loop_message) {
                        on_message_loop_message(msg);
                    }

                    if(msg.message == WM_QUIT)
                        return;
                }
            }
        }
    }
}