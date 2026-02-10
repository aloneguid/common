#pragma once
#include <string>
#include <vector>
#include <Windows.h>

namespace win32::os {

    // general

    /**
     * @brief Calls ::GetLastError() and returns the error text.
     * @return
     */
    std::string get_last_error_text();

    std::string get_computer_name();

    std::string get_user_name();

    std::string create_guid();

    /**
 * @brief Get current CPU usage in percentage
 * @return value 0-100, or -1 in case of failure
*/
    static double get_cpu_usage_perc();

    // process

    int create_process(const std::string& cmdline,
       std::string& std_out,
       std::string& std_err,
       std::string& sys_error_text);

    // peripheral

    /**
     * @brief Reads current keyboard state and returns text representation of currently pressed keys. For example "Ctrl+Shift+S".
     * @return 
     */
    std::string get_pressed_keys_text();

    // clipboard

    static void set_clipboard_text(const std::string& text);

    static std::string get_clipboard_text();
}