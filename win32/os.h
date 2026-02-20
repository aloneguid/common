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

    bool get_mouse_pos(long& x, long& y);

    // clipboard

    static void set_clipboard_text(const std::string& text);

    static std::string get_clipboard_text();

    // imaging

    bool capture_screen(int& width, int& height, std::vector<unsigned char>& out_pixels);

    /**
     * @brief Captures screen area and copies it to clipboard as bitmap.
     *        When x/y/w/h are all 0 (default), captures the entire screen.
     * @param x Left coordinate of the area (pixels)
     * @param y Top coordinate of the area (pixels)
     * @param w Width of the area (pixels, 0 = full screen)
     * @param h Height of the area (pixels, 0 = full screen)
     * @return true on success, false on failure
     */
    bool capture_screen_to_clipboard(int x = 0, int y = 0, int w = 0, int h = 0);
}