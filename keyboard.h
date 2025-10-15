#pragma once

#include <string>
#include <vector>
//#include <Windows.h>

namespace keyboard {

    enum class key_direction {
        press,
        down,
        up
    };

     wchar_t vk_to_wchar(unsigned long vk_code);

     void type_code(unsigned long code, key_direction dir = key_direction::press);

     void type_char(wchar_t wc);

     void type_backspace(int times, int delay_ms = 10);

     void send_backspace(int times);

     void type_text(const std::wstring& text, int delay_ms = 10);

     void send_text(const std::wstring& text);

     std::string to_string(unsigned long vk_code);

}