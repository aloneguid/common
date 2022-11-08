#include "clipboard.h"
#include <WinUser.h>
#include "../str.h"

using namespace std;

namespace win32 {

    // links:
    // - https://learn.microsoft.com/en-us/windows/win32/dataxchg/using-the-clipboard

    clipboard::clipboard() {
        ::RegisterClipboardFormat(L"FileNameW");
    }

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

    std::string clipboard::get_text() {
        return get_unicode_text(CF_UNICODETEXT);
    }

    std::string clipboard::get_filename() {
        return get_unicode_text(::RegisterClipboardFormat(L"FileNameW"));
    }

    std::string clipboard::get_unicode_text(UINT format) {
        if (!::IsClipboardFormatAvailable(format)) return "";
        if (!OpenClipboard(nullptr)) return "";
        wstring r;

        HGLOBAL gh = ::GetClipboardData(format);
        if (gh != NULL) {
            LPWSTR lpstr = (LPWSTR)::GlobalLock(gh);
            if (lpstr != NULL) {

                r = lpstr;

                ::GlobalUnlock(lpstr);
            }
        }

        ::CloseClipboard();

        return str::to_str(r);
    }
}