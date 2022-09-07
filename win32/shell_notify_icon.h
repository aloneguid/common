#pragma once
#include <Windows.h>

namespace win32 {
    class shell_notify_icon {
    public:
        shell_notify_icon(HWND hwnd);
        ~shell_notify_icon();


    private:
        HWND hwnd;
        GUID icon_guid;
    };
}