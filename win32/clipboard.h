#pragma once
#include <string>

namespace win32 {
    class clipboard {
    public:
        static void set_ascii_text(const std::string& text);
        static std::string get_ascii_text();
        static std::wstring get_text();
    };
}