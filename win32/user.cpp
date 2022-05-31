#include "user.h"
#include <WinUser.h>

using namespace std;

#define MAX_LOADSTRING 1024

// Version.lib is required to call ::GetFileVersionInfoSize etc.
#pragma comment(lib, "Version.lib")

namespace win32::user
{
   std::wstring load_string(HINSTANCE hInst, UINT id)
   {
      WCHAR szs[MAX_LOADSTRING];
      ::LoadString(hInst, id, szs, MAX_LOADSTRING);
      return wstring(szs);
   }

   std::wstring load_version(bool full)
   {
      wstring r;

      // get current module's filename
      TCHAR szVersionFile[MAX_PATH];
      ::GetModuleFileName(nullptr, szVersionFile, MAX_PATH);

      DWORD hVer = 0;
      DWORD verSize = ::GetFileVersionInfoSize(szVersionFile, &hVer);
      LPSTR verData = new char[verSize];

      if (::GetFileVersionInfo(szVersionFile, hVer, verSize, verData))
      {
         LPBYTE lpBuffer = 0;
         UINT size = 0;
         if (VerQueryValue(verData, L"\\", (VOID FAR * FAR*) & lpBuffer, &size))
         {
            if (size)
            {
               VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
               if (verInfo->dwSignature == 0xfeef04bd)
               {
                  // Doesn't matter if you are on 32 bit or 64 bit,
                  // DWORD is always 32 bits, so first two revision numbers
                  // come from dwFileVersionMS, last two come from dwFileVersionLS

                  r = to_wstring((verInfo->dwFileVersionMS >> 16) & 0xffff) + L"." +
                     to_wstring((verInfo->dwFileVersionMS >> 0) & 0xffff);

                  if (full)
                  {
                     r += L"." +
                        to_wstring((verInfo->dwFileVersionLS >> 16) & 0xffff) + L"." +
                        to_wstring((verInfo->dwFileVersionLS >> 0) & 0xffff);
                  }
               }
            }
         }

         delete[] verData;
      }

      return r;

   }

   void set_window_pos(HWND hwnd, int x, int y, int width, int height)
   {
      UINT flags{};

      if (x == -1 || y == -1) flags |= SWP_NOMOVE;
      if (width == -1 || height == -1) flags |= SWP_NOSIZE;

      ::SetWindowPos(hwnd, NULL, x, y, width, height, flags);
   }

   bool is_kbd_ctrl_down()
   {
      // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

      return ::GetAsyncKeyState(VK_CONTROL);
   }

   bool is_kbd_alt_down()
   {
      return ::GetAsyncKeyState(VK_LMENU) || ::GetAsyncKeyState(VK_RMENU);
   }

   bool is_kbd_shift_down()
   {
      return ::GetAsyncKeyState(VK_SHIFT);
   }

   void set_clipboard_data(const std::string& text)
   {
      if (!::OpenClipboard(nullptr)) return;
      if (!::EmptyClipboard()) return;
      HGLOBAL gh = ::GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
      if (!gh)
      {
         ::CloseClipboard();
         return;
      }
      memcpy(::GlobalLock(gh), text.c_str(), text.size() + 1);
      ::GlobalUnlock(gh);
      ::SetClipboardData(CF_TEXT, gh);
      ::CloseClipboard();
      ::GlobalFree(gh);
   }

}