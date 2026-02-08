#include "os.h"
#include <format>
#include "../str.h"
#include <Windows.h>
#include <VersionHelpers.h>
#include <combaseapi.h>

namespace win32::os {
    using namespace std;

    std::string get_last_error_text() {
        wchar_t err[256];
        memset(err, 0, 256);
        ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,
           GetLastError(),
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
        return str::to_str(wstring(err));
    }

    std::string get_computer_name() {
        TCHAR buffer[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

        ::GetComputerName(buffer, &size);

        return str::to_str(buffer);
    }

    std::string get_user_name() {
        TCHAR buffer[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

        ::GetUserName(buffer, &size);

        return str::to_str(buffer);
    }

    string pipe_read(HANDLE hReadPipe) {
        string result;
        vector<char> buffer;
        buffer.resize(1024);
        DWORD read{0};

        while(true) {

            bool read_ok = ::ReadFile(hReadPipe,
               (void*)&buffer[0], buffer.size(),
               &read, NULL);

            if(!read_ok || read == 0) {
                break;
            }

            std::copy(buffer.begin(), buffer.begin() + read, back_inserter(result));
        }

        return result;
    }

    int create_process(const std::string& cmdline,
       std::string& std_out,
       std::string& std_err,
       std::string& sys_error_text) {
        // https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw

        STARTUPINFO si{};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi{0};

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
        if(!::CreatePipe(&hStdInRead, &hStdInWrite, &saAttr, 0))
            return -1;
        if(!::CreatePipe(&hStdErrRead, &hStdErrWrite, &saAttr, 0))
            return -1;
        if(!::CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0))
            return -1;

        if(!::SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0))
            return -2;
        if(!::SetHandleInformation(hStdErrRead, HANDLE_FLAG_INHERIT, 0))
            return -2;
        if(!::SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0))
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

        if(created) {
            ::WaitForSingleObject(pi.hProcess, INFINITE);

            // not closing the handles before reading from out pipes causes read to hang at the end
            ::CloseHandle(pi.hProcess);
            ::CloseHandle(pi.hThread);

            // close sides of the pipes we are not using
            ::CloseHandle(hStdErrWrite);
            ::CloseHandle(hStdOutWrite);
            ::CloseHandle(hStdInRead);

            std_out = pipe_read(hStdOutRead);
            std_err = pipe_read(hStdErrRead);

            DWORD exit_code{0};
            ::GetExitCodeProcess(pi.hProcess, &exit_code);
            return static_cast<int>(exit_code);
        } else {
            sys_error_text = get_last_error_text();
        }

        return -3;
    }

    std::string create_guid() {
        GUID guid{0};
        ::CoCreateGuid(&guid);
        char guid_cstr[39];
        snprintf(guid_cstr, sizeof(guid_cstr),
                 "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                 guid.Data1, guid.Data2, guid.Data3,
                 guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
                 guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

        return guid_cstr;
    }

    double get_cpu_usage_perc() {
        FILETIME idleTime, kernelTime, userTime;
        if(GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
            ULARGE_INTEGER idleTimeU, kernelTimeU, userTimeU;
            idleTimeU.LowPart = idleTime.dwLowDateTime;
            idleTimeU.HighPart = idleTime.dwHighDateTime;
            kernelTimeU.LowPart = kernelTime.dwLowDateTime;
            kernelTimeU.HighPart = kernelTime.dwHighDateTime;
            userTimeU.LowPart = userTime.dwLowDateTime;
            userTimeU.HighPart = userTime.dwHighDateTime;

            ULONGLONG systemTime = kernelTimeU.QuadPart + userTimeU.QuadPart;
            ULONGLONG idleTime = idleTimeU.QuadPart;

            static ULONGLONG prevSystemTime = 0;
            static ULONGLONG prevIdleTime = 0;

            ULONGLONG systemTimeDiff = systemTime - prevSystemTime;
            ULONGLONG idleTimeDiff = idleTime - prevIdleTime;

            double cpuUsage = 100.0 - ((idleTimeDiff * 100.0) / systemTimeDiff);

            prevSystemTime = systemTime;
            prevIdleTime = idleTime;

            return cpuUsage;
        }
        return -1.0; // Failed to retrieve CPU usage
    }


    string get_pressed_keys_text() {
        vector<string> pressed_keys;

        // get entire keyboard state
        BYTE keyboard_state[256];
        if(::GetKeyboardState(keyboard_state)) {
            for(int i = 0; i < 256; i++) {
                // "i" is VK itself
                if(keyboard_state[i] & 0x80) {
                    // key is pressed
                    
                    // modifier keys
                    if(i == VK_CONTROL) {
                        pressed_keys.push_back("Ctrl");
                    } else if(i == VK_MENU) {
                        pressed_keys.push_back("Alt");
                    } else if(i == VK_SHIFT) {
                        pressed_keys.push_back("Shift");
                    } else if(i >= '0' && i <= '9') {   // numbers
                        pressed_keys.push_back(string(1, static_cast<char>(i)));
                    } else if(i >= 'A' && i <= 'Z') {   // letters
                        pressed_keys.push_back(string(1, static_cast<char>(i)));
                    } else if(i >= VK_F1 && i <= VK_F12) { // F keys
                        pressed_keys.push_back(format("F{}", i - VK_F1 + 1));
                    } else {
                        // other keys
                    }
                }
            }
        }

        // return pressed_keys joined by "+" sign
        string r;
        for(string& s : pressed_keys) {
            if(!r.empty()) {
                r += "+";
            }
            r += s;
        }
        return r;
    }
}