#include "process.h"
#include <chrono>
#include <filesystem>
#include <format>
#include "str.h"

using namespace std;
using namespace std::chrono;

#if PLATFORM_WINDOWS

#include <Pdh.h>
#include <PdhMsg.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <winnt.h>
#include <winternl.h>

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "version.lib")

#define MAX_STR 1024
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define MAX_EXTENDED_PATH 32768

process::process() : pid{ ::GetCurrentProcessId() } {

}

process::~process() {
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

DWORD process::start(const std::string& cmdline, bool wait_for_exit) {

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

        if(wait_for_exit) {
            ::WaitForSingleObject(pi.hProcess, INFINITE);
        }

        pid = pi.dwProcessId;

        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
    }

    return pid;
}

int process::exec(const std::string& cmdline, std::function<void(std::string&)> std_out_new_data) {
    HANDLE hStdOutRead = nullptr, hStdOutWrite = nullptr;
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE; // Allow the child process to inherit the handle
    sa.lpSecurityDescriptor = nullptr;

    // Create a pipe for the child process's standard output
    if(!::CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) {
        return -1; // Failed to create pipe
    }

    // Ensure the read handle to the pipe is not inherited
    if(!::SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0)) {
        ::CloseHandle(hStdOutRead);
        ::CloseHandle(hStdOutWrite);
        return -1; // Failed to set handle information
    }

    STARTUPINFO si{};
    si.cb = sizeof(STARTUPINFO);
    si.hStdOutput = hStdOutWrite; // Redirect standard output to the writing end of the pipe
    si.hStdError = hStdOutWrite;  // Redirect standard error to the same pipe
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi{};
    DWORD exit_code = -1;

    // Convert the command line to a wide string
    std::wstring wCmdline = str::to_wstr(cmdline);

    // Create the child process
    if(::CreateProcess(
        nullptr,
        const_cast<wchar_t*>(wCmdline.c_str()),
        nullptr,
        nullptr,
        TRUE, // Inherit handles
        0,
        nullptr,
        nullptr,
        &si,
        &pi)) {
        // Close the write end of the pipe in the parent process
        ::CloseHandle(hStdOutWrite);

        // Read the output from the child process
        char buffer[4096];
        DWORD bytesRead;
        while(::ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
            buffer[bytesRead] = '\0'; // Null-terminate the buffer
            string token{buffer};
            std_out_new_data(token);
        }

        // Wait for the child process to exit
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        // Get the exit code of the process
        ::GetExitCodeProcess(pi.hProcess, &exit_code);

        // Close handles
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
    } else {
        // Failed to create process
        ::CloseHandle(hStdOutWrite);
    }

    ::CloseHandle(hStdOutRead);
    return static_cast<int>(exit_code);
}

int process::exec(const std::string& cmdline, string& std_out) {
    return exec(cmdline, [&](string& token) {
        std_out += token;
    });
}

std::string process::get_module_filename() const {

    // Try the modern API first: needs only LIMITED_INFORMATION,
    // works on elevated/protected processes, handles long paths.
    {
        HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if(hProcess) {
            wchar_t buf[32768];
            DWORD len = static_cast<DWORD>(std::size(buf));
            if(::QueryFullProcessImageNameW(hProcess, 0, buf, &len)) {
                ::CloseHandle(hProcess);
                return str::to_str(std::wstring(buf, len));
            }
            ::CloseHandle(hProcess);
        }
    }

    // Fall back to the older API.
    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if(!hProcess) return "";

    wchar_t buf[MAX_PATH];
    std::string r;
    if(::GetModuleFileNameExW(hProcess, nullptr, buf, MAX_PATH)) {
        r = str::to_str(std::wstring(buf));
    }
    ::CloseHandle(hProcess);
    return r;
}

std::string process::get_name() const {
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

std::string process::get_description() const {

    // --- 1. Get executable path ---
    std::wstring exePath = str::to_wstr(get_module_filename());
    if(exePath.empty()) return "";

    // --- 2. Load version-info block ---
    DWORD dummy = 0;
    DWORD viSize = ::GetFileVersionInfoSizeW(exePath.c_str(), &dummy);
    if(viSize == 0) return "";

    std::vector<BYTE> vi(viSize);
    if(!::GetFileVersionInfoW(exePath.c_str(), 0, viSize, vi.data())) return "";

    // --- 3. Enumerate translations actually present in this binary ---
    struct LANGANDCODEPAGE { WORD language; WORD codepage; };

    LANGANDCODEPAGE* translations = nullptr;
    UINT translationBytes = 0;
    ::VerQueryValueW(vi.data(), L"\\VarFileInfo\\Translation",
                        reinterpret_cast<void**>(&translations), &translationBytes);

    const UINT translationCount = translationBytes / sizeof(LANGANDCODEPAGE);

    auto queryDescription = [&](WORD lang, WORD cp) -> std::wstring {
        wchar_t subBlock[64];
        ::swprintf_s(subBlock, L"\\StringFileInfo\\%04X%04X\\FileDescription",
                        lang, cp);
        void* buf = nullptr;
        UINT  sz = 0;
        if(::VerQueryValueW(vi.data(), subBlock, &buf, &sz) && sz > 0) {
            return std::wstring(static_cast<wchar_t*>(buf));
        }
        return {};
    };

    for(UINT i = 0; i < translationCount; ++i) {
        auto desc = queryDescription(translations[i].language,
                                        translations[i].codepage);
        if(!desc.empty()) return str::to_str(desc);
    }

    // --- 4. Try common fallback combos that some tools emit without declaring ---
    for(auto [lang, cp] : std::initializer_list<std::pair<WORD, WORD>>{
            {0x0409, 0x04B0},
            {0x0409, 0x04E4},
            {0x0000, 0x04B0},
    }) {
        auto desc = queryDescription(lang, cp);
        if(!desc.empty()) return str::to_str(desc);
    }

    return "";
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
    HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return process{0};

    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(entry); // must be set before Process32First, or the call fails

    DWORD parentPid = 0;
    if (::Process32First(snapshot, &entry)) {
        do {
            if (entry.th32ProcessID == pid) {
                parentPid = entry.th32ParentProcessID;
                break;
            }
        } while (::Process32Next(snapshot, &entry));
    }

    ::CloseHandle(snapshot);
    return process{parentPid};
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

            auto path = str::to_wstr(std::format("\\Process V2({}:8520)\\% Processor Time", process_name));
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

#else

#endif