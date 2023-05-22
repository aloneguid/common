#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace win32 {
    class process {
    public:
        process(DWORD pid) : pid{pid} {

        }

        process();

        static std::vector<process> enumerate();

        std::string get_module_filename();
        std::string get_name();

        HWND find_main_window();

        void set_priority(DWORD priority_class = NORMAL_PRIORITY_CLASS);

        bool enable_efficiency_mode();

        bool is_valid() { return pid != 0; }

        process get_parent();

        bool get_memory_info(uint64_t& working_set_bytes);

        double get_uptime_sec();

        double get_cpu_usage();

        bool terminate();

        bool suspend();

        bool resume();

        bool is_suspended();

    private:
        DWORD pid;
    };
}