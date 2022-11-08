#pragma once
#include <Windows.h>
#include <string>

namespace win32 {
    class clipboard {
    public:
        clipboard();

        static void set_ascii_text(const std::string& text);

        static std::string get_text();
        static std::string get_filename();

    private:
        static std::string get_unicode_text(UINT format);
    };
}