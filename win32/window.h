#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <memory>

namespace win32 {
    class window {
    public:
        window(HWND hwnd) : hwnd{ hwnd } { }

        static std::vector<window> enumerate();

        std::string get_text();

        bool is_visible();

        std::shared_ptr<window> get_owner();

        DWORD get_pid();

    private:
        HWND hwnd;
    };
}