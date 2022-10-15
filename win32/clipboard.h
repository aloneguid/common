#pragma once
#include <string>

namespace win32 {
    class clipboard {
    public:
        static void set_text(const std::string& text);
        static std::string get_text();
    };
}