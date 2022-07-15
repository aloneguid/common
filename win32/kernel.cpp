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

   string pipe_read(HANDLE hReadPipe)
   {
      string result;
      vector<char> buffer;
      buffer.resize(1024);
      DWORD read{ 0 };

      while (true)
      {
         
         bool read_ok = ::ReadFile(hReadPipe,
            (void*)&buffer[0], buffer.size(),
            &read, NULL);

         if(!read_ok || read == 0)
         {
            break;
         }

         std::copy(buffer.begin(), buffer.begin() + read, back_inserter(result));
      }

      return result;
   }

   int kernel::create_process(const std::string& cmdline,
      std::string& std_out,
      std::string& std_err)
   {
      // https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw

      STARTUPINFO si{};
      si.cb = sizeof(si);
      PROCESS_INFORMATION pi{ 0 };

      wchar_t rcmd[MAX_PATH];
      ::ZeroMemory(rcmd, sizeof(wchar_t) * MAX_PATH);
      wstring wcmdline = str::to_wstr(cmdline);
      memcpy(rcmd, &wcmdline[0], sizeof(wchar_t) * cmdline.size());

      // output redirection
      // https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output

      SECURITY_ATTRIBUTES saAttr{};
      saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
      saAttr.bInheritHandle = TRUE;
      saAttr.lpSecurityDescriptor = NULL;

      HANDLE hStdInRead, hStdInWrite;
      HANDLE hStdErrRead, hStdErrWrite;
      HANDLE hStdOutRead, hStdOutWrite;
      if (!::CreatePipe(&hStdInRead, &hStdInWrite, &saAttr, 0))
         return -1;
      if (!::CreatePipe(&hStdErrRead, &hStdErrWrite, &saAttr, 0))
         return -1;
      if (!::CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0))
         return -1;

      if (!::SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0))
         return -2;
      if (!::SetHandleInformation(hStdErrRead, HANDLE_FLAG_INHERIT, 0))
         return -2;
      if (!::SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0))
         return -2;

      si.hStdInput = hStdInRead;
      si.hStdError = hStdErrWrite;
      si.hStdOutput = hStdOutWrite;
      si.dwFlags |= STARTF_USESTDHANDLES;

      // https://docs.microsoft.com/en-us/windows/win32/procthread/creating-processes
      BOOL created = ::CreateProcess(nullptr,
         rcmd,
         nullptr, nullptr,
         true,
         0,
         nullptr,
         nullptr,
         &si,
         &pi);

      if (created)
      {
         // not closing the handles before reading from out pipes causes read to hang at the end
         ::CloseHandle(pi.hProcess);
         ::CloseHandle(pi.hThread);

         ::CloseHandle(hStdErrWrite);  // not writing out ERR anything from here
         ::CloseHandle(hStdOutWrite);  // not writing out OUT anything from here
         ::CloseHandle(hStdInRead); // not reading from IN anything from here

         std_out = pipe_read(hStdOutRead);
         std_err = pipe_read(hStdErrRead);

         DWORD exit_code{};
         ::GetExitCodeProcess(pi.hProcess, &exit_code);
         return static_cast<int>(exit_code);
      }

      return -3;
   }
}