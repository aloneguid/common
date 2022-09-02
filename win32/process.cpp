#include "process.h"
#include "../str.h"
#include <Psapi.h>

#define MAX_STR 1024

using namespace std;

namespace win32 {
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
}