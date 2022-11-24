#pragma once
#include <windows.h>
#include <string>
#include <vector>

namespace win32::user
{
   std::wstring load_string(HINSTANCE hInst, UINT id);

   std::wstring load_version(bool full = true);

   // see string_name possible values in Remarks:
   // https://docs.microsoft.com/en-us/windows/win32/api/winver/nf-winver-verqueryvaluea#remarks
   std::wstring get_file_version_info_string(
       const std::string& module_path,
       const std::string& string_name = "FileDescription");

   /// <summary>
   /// Sets window position and size.
   /// To keep current position, pass -1 to x and y.
   /// To keep current size, pass -1 to width and height.
   /// </summary>
   void set_window_pos(HWND hwnd, int x, int y, int width, int height);

   bool is_kbd_ctrl_down();
   bool is_kbd_alt_down();
   bool is_kbd_shift_down();

   // returns true if app theme is set to Light
   bool is_app_light_theme(bool& value);

   // return true if system theme is set to Light
   bool is_system_light_theme(bool& value);
}

