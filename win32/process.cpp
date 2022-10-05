#include "process.h"
#include "../str.h"
#include <Psapi.h>
#include <winnt.h>
#include <winternl.h>

#define MAX_STR 1024

using namespace std;

namespace win32 {

    typedef NTSTATUS(__stdcall* NTQUERYINFORMATIONPROCESS) (
        HANDLE ProcessHandle,
        PROCESSINFOCLASS ProcessInformationClass,
        PVOID ProcessInformation,
        ULONG ProcessInformationLength,
        PULONG ReturnLength);

    HMODULE ntdll = GetModuleHandle(L"NTDLL.DLL");

    NTQUERYINFORMATIONPROCESS ptrNtQueryInformationProcess =
        (NTQUERYINFORMATIONPROCESS)::GetProcAddress(ntdll, "NtQueryInformationProcess");

    process::process() : pid{ ::GetCurrentProcessId() } {

    }

    vector<process> process::enumerate() {
        vector<process> r;

        DWORD process_ids[1024], needed;
        if (::EnumProcesses(process_ids, sizeof(process_ids), &needed)) {
            int proc_count = needed / sizeof(DWORD);
            for (int i = 0; i < proc_count; i++) {
                auto process_id = process_ids[i];

                r.emplace_back(process_id);
            }
        }

        return r;
    }

    std::string process::get_module_filename() {

        string r;

        HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
        if (hProcess) {
            WCHAR buf[MAX_STR];

            if (::GetModuleFileNameEx(hProcess, NULL, buf, MAX_STR)) {
                r = str::to_str(wstring(buf));
            }
            ::CloseHandle(hProcess);
        }

        return r;
    }

    std::string process::get_name() {
        string mfn = get_module_filename();

        size_t idx = mfn.find_last_of('\\');
        if (idx != string::npos) {
            mfn = mfn.substr(idx + 1);
        }

        return mfn;
    }

    struct find_token {
        HWND best_handle{ 0 };
        DWORD process_id{ 0 };
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

    HWND process::find_main_window() {
        find_token t;
        t.process_id = pid;

        ::EnumWindows(FindMainWindowEnumWindowsProc, (LPARAM)&t);

        return t.best_handle;
    }

    void process::set_priority(DWORD priority_class) {
        HANDLE hProcess = ::OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
        if(hProcess) {
            ::SetPriorityClass(hProcess, priority_class);

            ::CloseHandle(hProcess);
        }
    }

    bool process::enable_efficiency_mode() {
        BOOL ok = FALSE;
        HANDLE hProcess = ::OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
        if (hProcess) {

            PROCESS_POWER_THROTTLING_STATE pts = { 0 };
            pts.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
            pts.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
            pts.StateMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;

            ok = ::SetProcessInformation(hProcess, ProcessPowerThrottling, &pts, sizeof(pts));

            ::CloseHandle(hProcess);
        }
        return ok;
    }

    process process::get_parent() {
        if (ptrNtQueryInformationProcess != 0) {
            PROCESS_BASIC_INFORMATION processBasicInformation;
            ULONG retLength = 0;
            NTSTATUS status = ptrNtQueryInformationProcess(
                ::GetCurrentProcess(),
                ProcessBasicInformation,
                &processBasicInformation,
                sizeof(processBasicInformation),
                &retLength);

            if (status == 0) {
                DWORD parent_id = (DWORD)processBasicInformation.Reserved3;

                return process{ parent_id };
            }
        }

        return process{ 0 };
    }
}