#pragma once
#include <Windows.h>

namespace win32 {
    class shell_notify_icon {
    public:
        shell_notify_icon(HWND hwnd, UINT callback_message = WM_APP + 1);
        ~shell_notify_icon();


    private:
        HWND hwnd;
        GUID icon_guid;
        UINT callback_message;
    };
}