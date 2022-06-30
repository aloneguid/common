#include "kernel.h"
#include "../str.h"
#include <Windows.h>
#include <fmt/core.h>

namespace win32
{
   using namespace std;

   std::string kernel::get_computer_name()
   {
      TCHAR buffer[MAX_COMPUTERNAME_LENGTH + 1];
      DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

      ::GetComputerName(buffer, &size);

      return str::to_str(buffer);
   }

   std::string kernel::get_user_name()
   {
      TCHAR buffer[MAX_COMPUTERNAME_LENGTH + 1];
      DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

      ::GetUserName(buffer, &size);

      return str::to_str(buffer);
   }

   std::string kernel::get_os_version()
   {
      DWORD dwVersion = 0;
      DWORD dwMajorVersion = 0;
      DWORD dwMinorVersion = 0;
      DWORD dwBuild = 0;

      dwVersion = ::GetVersion();

      // Get the Windows version.

      dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
      dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

      // Get the build number.

      if (dwVersion < 0x80000000)
         dwBuild = (DWORD)(HIWORD(dwVersion));

      return fmt::format("{}.{}.{}", dwMajorVersion, dwMinorVersion, dwBuild);
   }
}