#include "ps.h"
#include <Psapi.h>

#define MAX_STR 1024

namespace win32
{
   void ps::get_process_memory_info() {
      //todo: not working yet
      HANDLE hProcess = ::GetCurrentProcess();

      PROCESS_MEMORY_COUNTERS pmc;
      BOOL ok = ::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));

      size_t wss = pmc.WorkingSetSize;
   }

   std::wstring ps::get_window_process_module_file_name(HWND hwnd) {

       DWORD pid;
       if (::GetWindowThreadProcessId(hwnd, &pid)) {
           HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
           if (!hProcess) return L"";

           WCHAR buf[MAX_STR];

           if (::GetModuleFileNameEx(hProcess, NULL, buf, MAX_STR)) {
               return std::wstring(buf);
           }
       }

       return L"";
   }

   std::vector<DWORD> ps::enumerate_processes_ids() {

       std::vector<DWORD> result;

       DWORD process_ids[1024], needed;
       if (::EnumProcesses(process_ids, sizeof(process_ids), &needed)) {
           int proc_count = needed / sizeof(DWORD);
           for (int i = 0; i < proc_count; i++) {
               auto process_id = process_ids[i];

               result.push_back(process_id);
           }
       }

       return result;
   }

   struct find_token {
       HWND best_handle { 0 };
       DWORD process_id { 0 };
   };

   BOOL CALLBACK FindMainWindowEnumWindowsProc(HWND hwnd, LPARAM lParam) {

       find_token* t = (find_token*)lParam;

       HWND owner = ::GetWindow(hwnd, GW_OWNER);
       bool is_visible = ::IsWindowVisible(hwnd);
       bool is_main_window = !owner && is_visible;

       DWORD process_id;
       ::GetWindowThreadProcessId(hwnd, &process_id);

       if (process_id != t->process_id || !is_main_window)
           return TRUE;

       t->best_handle = hwnd;
       return FALSE;
   }

   HWND ps::find_main_window(DWORD process_id) {

       find_token t;
       t.process_id = process_id;

       ::EnumWindows(FindMainWindowEnumWindowsProc, (LPARAM)&t);

       return t.best_handle;
   }
}