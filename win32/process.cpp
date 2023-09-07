#include "process.h"
#include "../str.h"
#include <Psapi.h>
#include <winnt.h>
#include <winternl.h>
#include <chrono>
#include "kernel.h"
#include "str.h"
#include <fmt/core.h>
#include <Pdh.h>
#include <PdhMsg.h>

#define MAX_STR 1024
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)

using namespace std;
using namespace std::chrono;

namespace win32 {

    // interesting findings: https://github.com/RandomOS/taskmanager/blob/master/src/process.c

    typedef NTSTATUS(__stdcall* NTQUERYINFORMATIONPROCESS) (
        HANDLE ProcessHandle,
        PROCESSINFOCLASS ProcessInformationClass,
        PVOID ProcessInformation,
        ULONG ProcessInformationLength,
        PULONG ReturnLength);

    typedef NTSTATUS(NTAPI* NtQuerySystemInformation)(
        IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
        OUT PVOID SystemInformation,
        IN ULONG SystemInformationLength,
        OUT PULONG ReturnLength);

    HMODULE ntdll = ::GetModuleHandle(L"NTDLL.DLL");

    // NtQueryInformationProcess
    NTQUERYINFORMATIONPROCESS ptrNtQueryInformationProcess =
        (NTQUERYINFORMATIONPROCESS)::GetProcAddress(ntdll, "NtQueryInformationProcess");

    // NtQuerySystemInformation
    NtQuerySystemInformation ptrNtQuerySystemInformation = (NtQuerySystemInformation)::GetProcAddress(
        ntdll, "NtQuerySystemInformation");

    // NtSuspendProcess / NtResumeProcess
    typedef NTSTATUS(NTAPI* NtSuspendProcess)(IN HANDLE ProcessHandle);

    typedef NTSTATUS(NTAPI* NtResumeProcess)(IN HANDLE ProcessHandle);

    NtSuspendProcess ptrNtSuspendProcess = (NtSuspendProcess)::GetProcAddress(
        ntdll, "NtSuspendProcess");

    NtResumeProcess ptrNtResumeProcess = (NtResumeProcess)::GetProcAddress(
        ntdll, "NtResumeProcess");

    process::process() : pid{ ::GetCurrentProcessId() } {

    }

    process::~process()
    {
        if(pdhCpuInitialised) {
            ::PdhRemoveCounter(pdhCpuCounter);
            ::PdhCloseQuery(pdhCpuQuery);
        }
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

    DWORD process::start(const std::string& cmdline) {

        STARTUPINFO si{};
        PROCESS_INFORMATION pi{};
        DWORD pid{ 0 };

        if (::CreateProcess(nullptr,
            const_cast<wchar_t*>(str::to_wstr(cmdline).c_str()),
            nullptr,
            nullptr,
            false,
            0,
            nullptr,
            nullptr,
            &si,
            &pi)) {

            ::WaitForSingleObject(pi.hProcess, INFINITE);

            pid = pi.dwProcessId;

            ::CloseHandle(pi.hProcess);
            ::CloseHandle(pi.hThread);
        }

        return pid;
    }

    std::string process::get_module_filename() const {

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

    bool process::get_memory_info(uint64_t& working_set_bytes) {
        bool ok{false};
        // see https://learn.microsoft.com/en-us/windows/win32/api/psapi/nf-psapi-getprocessmemoryinfo
        HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if(hProcess) {
            PROCESS_MEMORY_COUNTERS pmc{0};
            ::GetProcessMemoryInfo(hProcess, &pmc, sizeof(PROCESS_MEMORY_COUNTERS));
            working_set_bytes = pmc.WorkingSetSize;

            ::CloseHandle(hProcess);

            ok = true;
        }
        return ok;
    }

    double process::get_uptime_sec() {
        double r{0};
        HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if(hProcess) {
            FILETIME creationTime, exitTime, kernelTime, userTime;
            if(::GetProcessTimes(
                hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {

                // convert to time_point
                file_clock::duration d{
                    (static_cast<int64_t>(creationTime.dwHighDateTime) << 32) | creationTime.dwLowDateTime};
                file_clock::time_point creation_time{d};

                // get duration
                auto uptime = file_clock::now() - creation_time;
                auto uptime_sec = duration_cast<seconds>(uptime);
                r = uptime_sec.count();
            }
            ::CloseHandle(hProcess);
        }
        return r;
    }

    bool process::terminate() {
        HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        bool ok = false;
        if (hProcess) {
            ok = ::TerminateProcess(hProcess, 1);
            ::CloseHandle(hProcess);
        }
        return ok;
    }

    bool process::suspend() {
        HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        NTSTATUS r = 0;
        if (hProcess) {
            r = ptrNtSuspendProcess(hProcess);
            ::CloseHandle(hProcess);
        }
        return NT_SUCCESS(r);
    }

    bool process::resume() {
        HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        NTSTATUS r = 0;
        if (hProcess) {
            r = ptrNtResumeProcess(hProcess);
            ::CloseHandle(hProcess);
        }
        return NT_SUCCESS(r);
    }

    bool process::is_suspended() {
        vector<char> pBuffer;
        size_t cbBuffer = 0x2800;
        pBuffer.resize(cbBuffer);
        NTSTATUS status{ 0 };
        do
        {
            status = ptrNtQuerySystemInformation(SystemProcessInformation, &pBuffer[0], cbBuffer, NULL);
            if (status == STATUS_INFO_LENGTH_MISMATCH) {
                cbBuffer *= 2;
                pBuffer.resize(cbBuffer);
            }
        } while (status == STATUS_INFO_LENGTH_MISMATCH);

        if (!NT_SUCCESS(status)) return false;

        PSYSTEM_PROCESS_INFORMATION p_info = (PSYSTEM_PROCESS_INFORMATION)&pBuffer[0];
        bool r = false;

        /*while (true) {
            if (p_info->UniqueProcessId == (HANDLE)pid) {
                r = p_info->NumberOfThreads != 0;
                for (int i = 0; i < p_info->NumberOfThreads; i++) {
                    p_info->thre
                    if (p_info->Threads[i] != StateWait || p_info->Threads[i].WaitReason != Suspended) {
                        r = false;
                        break;
                    }
                }
            }

            if (p_info->NextEntryDelta == 0) break;

            p_info = (PNT_SYSTEM_PROCESS_INFORMATION)(((PUCHAR)p_info) + p_info->NextEntryDelta);
        }*/

        return r;
    }

    double process::get_cpu_usage_percentage() {

        double perc{-1.0};

        // initialise pdh
        if (!pdhCpuInitialised) {

            auto status = ::PdhOpenQuery(NULL, NULL, &pdhCpuQuery);
            if (status == ERROR_SUCCESS) {
                // get process name and strip out extension
                string process_name = get_name();
                size_t idx = process_name.find_last_of('.');
                if(idx != string::npos) process_name = process_name.substr(0, idx);
                
                auto path = str::to_wstr(fmt::format("\\Process V2({}:8520)\\% Processor Time", process_name));
                status = ::PdhAddEnglishCounter(pdhCpuQuery, path.c_str(), NULL, &pdhCpuCounter);
                if(status == ERROR_SUCCESS) {
                    pdhCpuInitialised = true;
                } else {
                    ::PdhCloseQuery(pdhCpuQuery);
                    pdhCpuQuery = 0;
                }
            }
        }

        // get sample
        if(pdhCpuInitialised && pdhCpuQuery != 0) {
            auto status = ::PdhCollectQueryData(pdhCpuQuery);
            if(status == ERROR_SUCCESS) {
                // Retrieve the counter value
                PDH_FMT_COUNTERVALUE fv;
                status = ::PdhGetFormattedCounterValue(pdhCpuCounter, PDH_FMT_DOUBLE, NULL, &fv);
                if(ERROR_SUCCESS == status) {
                    perc = fv.doubleValue;
                } else {
                    if(status == PDH_INVALID_ARGUMENT) {
                        perc = -2;
                    } else if(status == PDH_INVALID_DATA) {
                        perc = -3;
                    } else if(status == PDH_INVALID_HANDLE) {
                        perc = -4;
                    }
                }
            }
        }

        return perc;
    }
}