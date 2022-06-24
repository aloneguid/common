#include "ps.h"
#include <Windows.h>
#include <Psapi.h>

namespace win32
{
   void ps::get_process_memory_info()
   {
      //todo: not working yet
      HANDLE hProcess = ::GetCurrentProcess();

      PROCESS_MEMORY_COUNTERS pmc;
      BOOL ok = ::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));

      size_t wss = pmc.WorkingSetSize;
   }
}