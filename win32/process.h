#pragma once
#include <Windows.h>
#include <string>

namespace win32 {
    class process {
    public:
        process(DWORD pid) : pid{ pid } {

        }

        std::string get_module_filename();
        std::string get_name();

    private:
        DWORD pid;
    };
}