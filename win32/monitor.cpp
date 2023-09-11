#include "monitor.h"
#include <WinUser.h>

using namespace std;

namespace win32 {

    BOOL monitor_list_enum_proc(
        HMONITOR hMonitor,
        HDC hDC,
        LPRECT pVirtualCoordinates,
        LPARAM hData) {

        vector<monitor>* r = (vector<monitor>*)hData;

        r->emplace_back(r->size(), hMonitor, "", *pVirtualCoordinates);

        return TRUE;
    }

    vector<monitor> monitor::list() {

        vector<monitor> r;

        ::EnumDisplayMonitors(
            nullptr,
            nullptr,
            monitor_list_enum_proc,
            (LPARAM)&r);

        return r;
    }

    monitor win32::monitor::get_nearest(HWND hwnd) {

        HMONITOR hMonitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        auto all = list();

        for(size_t i = 0; i < all.size(); i++) {
            if(hMonitor == all[i].h) return all[i];
        }

        return all[0];
    }
}