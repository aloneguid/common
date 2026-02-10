#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <memory>

namespace win32 {
    class window {
    public:
        window(HWND hwnd) : hwnd{ hwnd } { }

        window(const window& w) : hwnd{ w.get_handle() } { }

        bool is_valid();

        HWND get_handle() const { return hwnd; }

        static std::vector<window> enumerate();

        static window get_foreground();

        static window find_by_class_name(const std::string& class_name);

        std::string get_text();

        bool is_visible();

        std::shared_ptr<window> get_owner();

        DWORD get_pid() const;

        std::string get_class_name();

        LONG get_styles();

        /**
         * @brief Sets the window to be topmost (above all windows) or removes the flag.
         * @param topmost 
        */
        void set_topmost(bool topmost);

        // style related functions
        bool is_child();
        bool is_minimized();

        void restore();

        /**
         * @brief Minimize this window.Note that some windows like Task Manager(system app) do not respond to this.
         */
        void minimize();

        /**
         * @brief Mark the window as ecluded from video capture. This is useful to display in-app control that should not be recorded by screen recording software.
         * @param exclude set/unset.
         * @return True if operation was successful.
         */
        bool exclude_from_capture(bool exclude);

    private:
        HWND hwnd;
    };
}