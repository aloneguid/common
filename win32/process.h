#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace win32 {
    class process {
    public:
        process(DWORD pid) : pid{ pid } {

        }

        static std::vector<process> enumerate();

        std::string get_module_filename();
        std::string get_name();

        HWND find_main_window();

    private:
        DWORD pid;
    };
}