#pragma once
#include <Windows.h>
#include <string>

namespace win32 {
    class popup_menu {
    public:
        popup_menu(HWND h_wnd_owner);
        ~popup_menu();

        void add(const std::string& title);
        void separator();

        // Shows menu to the user.
        // When an item is selected, owner window receives WM_COMMAND with LOWORD(wParam) set to WM_APP + item index.
        void show();

        // Clears menu by recreating it in place
        void clear();

    private:
        size_t count{ 0 };
        UINT_PTR next_id{ 0 };
        HMENU h_menu;
        HWND h_wnd_owner;
    };
}