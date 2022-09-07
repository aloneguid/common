#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <memory>

namespace win32 {
    class window {
    public:
        window(HWND hwnd) : hwnd{ hwnd } { }

        window(const window& w) : hwnd{ w.get_handle() } { }

        bool is_valid();

        HWND get_handle() const { return hwnd; }

        static std::vector<window> enumerate();

        std::string get_text();

        bool is_visible();

        std::shared_ptr<window> get_owner();

        DWORD get_pid();

        std::string get_class_name();

        LONG get_styles();

        // style related functions
        bool is_child();
        bool is_minimized();

        void restore();

        void minimize();

    private:
        HWND hwnd;
    };
}