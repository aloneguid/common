#include "clipboard.h"
#include <Windows.h>
#include <WinUser.h>
#include "../str.h"

using namespace std;

namespace win32 {

    // links:
    // - https://learn.microsoft.com/en-us/windows/win32/dataxchg/using-the-clipboard

    void clipboard::set_ascii_text(const std::string& text) {
        if (!::OpenClipboard(nullptr)) return;
        if (!::EmptyClipboard()) return;
        HGLOBAL gh = ::GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (!gh) {
            ::CloseClipboard();
            return;
        }
        memcpy(::GlobalLock(gh), text.c_str(), text.size() + 1);
        ::GlobalUnlock(gh);
        ::SetClipboardData(CF_TEXT, gh);
        ::CloseClipboard();
        ::GlobalFree(gh);
    }

    std::string clipboard::get_ascii_text() {
        if (!::IsClipboardFormatAvailable(CF_TEXT)) return "";
        if (!OpenClipboard(nullptr)) return "";
        string r;

        HGLOBAL gh = ::GetClipboardData(CF_TEXT);
        if (gh != NULL) {
            LPSTR lpstr = (LPSTR)::GlobalLock(gh);
            if (lpstr != NULL) {

                r = lpstr;

                ::GlobalUnlock(lpstr);
            }
        }

        ::CloseClipboard();

        return r;
    }

    std::wstring clipboard::get_text() {
        if (!::IsClipboardFormatAvailable(CF_UNICODETEXT)) return L"";
        if (!OpenClipboard(nullptr)) return L"";
        wstring r;

        HGLOBAL gh = ::GetClipboardData(CF_UNICODETEXT);
        if (gh != NULL) {
            LPWSTR lpstr = (LPWSTR)::GlobalLock(gh);
            if (lpstr != NULL) {

                r = lpstr;

                ::GlobalUnlock(lpstr);
            }
        }

        ::CloseClipboard();

        return r;
    }
}