#pragma once
#include <string>
#include <vector>
#include <Windows.h>

namespace win32
{
   class ps
   {
   public:
      static void get_process_memory_info();

      static std::wstring get_window_process_module_file_name(HWND hwnd);

      static std::vector<DWORD> enumerate_processes_ids();

      static HWND find_main_window(DWORD process_id);
   };
}