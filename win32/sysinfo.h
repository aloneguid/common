#pragma once

namespace win32 {
    class system_info {
    public:
        /**
         * @brief Get current CPU usage in percentage
         * @return value 0-100, or -1 in case of failure
        */
        static double get_cpu_usage_perc();
    };
}