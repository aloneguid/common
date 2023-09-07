#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace win32 {
    class process {
    public:
        /**
         * @brief Creates process from a specific PID.
         * @param pid 
        */
        process(DWORD pid) : pid{pid} {

        }

        /**
         * @brief Creates process from current process.
        */
        process();

        ~process();

        static std::vector<process> enumerate();

        /**
         * @brief Starts a process and returns PID on success
         * @param cmdline 
         * @return PID
        */
        static DWORD start(const std::string& cmdline);

        std::string get_module_filename() const;
        std::string get_name();

        HWND find_main_window();

        void set_priority(DWORD priority_class = NORMAL_PRIORITY_CLASS);

        bool enable_efficiency_mode();

        bool is_valid() { return pid != 0; }

        process get_parent();

        bool get_memory_info(uint64_t& working_set_bytes);

        double get_uptime_sec();

        bool terminate();

        bool suspend();

        bool resume();

        bool is_suspended();

        /**
         * @brief Using sampling (windows performance counters) therefore accurate measurements will only happen after a second or so call with a second delay. This is using "Process V2" counter available on windows 10 and above.
         * @return 
        */
        double get_cpu_usage_percentage();

    private:
        DWORD pid;

        // cpu perf counter
        bool pdhCpuInitialised{false};
        HANDLE pdhCpuQuery{nullptr};
        HANDLE pdhCpuCounter{nullptr};

    };
}