#include "sysinfo.h"
#include <Windows.h>

namespace win32 {
    double system_info::get_cpu_usage_perc() {
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
}
