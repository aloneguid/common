#include "popup_menu.h"
#include "../str.h"

namespace win32 {
    popup_menu::popup_menu(HWND h_wnd_owner) 
        : h_wnd_owner{ h_wnd_owner } {
        clear();
    }

    popup_menu::~popup_menu() {
        clear();
        ::DestroyMenu(h_menu);
    }

    void popup_menu::add(const std::string& id, const std::string& title, bool disabled) {
        auto loword_wparam = next_id++;
        loword_wparam_to_id[loword_wparam] = id;
        UINT flags = MF_BYPOSITION;
        if (disabled) flags |= MF_DISABLED;
        BOOL ok = ::InsertMenu(*h_menu_nesting.rbegin(),
            -1,
            flags, 
            loword_wparam,
            str::to_wstr(title).c_str());

        // note: to have icons, entire menu item needs to be owner drawn :(

        /*MENUITEMINFO mii = {0};
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_STRING | MIIM_BITMAP;
        auto wtitle = str::to_wstr(title);
        mii.dwTypeData = (LPWSTR)(wtitle.c_str());
        //mii.hbmpItem = (HBITMAP)::LoadIcon(NULL, MAKEINTRESOURCE("IDI_ICON_EXIT"));
        mii.hbmpItem = ::LoadBitmap(h_inst, L"IDI_ICON1");
        ::InsertMenuItem(h_menu, -1, TRUE, &mii);*/

        count += 1;
    }

    void popup_menu::separator() {
        ::InsertMenu(*h_menu_nesting.rbegin(), -1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
    }

    void popup_menu::enter_submenu(const std::string& title) {
        HMENU h_sub = ::CreatePopupMenu();
        ::AppendMenu(*h_menu_nesting.rbegin(), MF_POPUP, (UINT)h_sub, str::to_wstr(title).c_str());
        h_menu_nesting.push_back(h_sub);
    }

    void popup_menu::exit_submenu() {
        if (h_menu_nesting.size() == 1) return;
        h_menu_nesting.erase(h_menu_nesting.end() - 1);
    }

    void popup_menu::clear() {
        for (int i = h_menu_nesting.size() - 1; i >= 0; i--) {
            ::DestroyMenu(h_menu_nesting[i]);
        }
        h_menu = ::CreatePopupMenu();
        h_menu_nesting.push_back(h_menu);
        count = 0;
        next_id = 0;
        loword_wparam_to_id.clear();
    }

    std::string popup_menu::id_from_loword_wparam(int loword_wparam) {
        return loword_wparam_to_id[loword_wparam];
    }

    void popup_menu::show() {

        if (count == 0) return;

        ::SetForegroundWindow(h_wnd_owner);

        POINT pt;
        ::GetCursorPos(&pt);
        ::TrackPopupMenu(h_menu, TPM_BOTTOMALIGN, pt.x, pt.y, 0, h_wnd_owner, NULL);
    }
}