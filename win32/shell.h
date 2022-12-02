#include <string>
#include <windows.h>

namespace win32 {
    namespace shell {
        struct shell_link {
            bool is_valid{ false };
            std::string path;
            std::string args;
            std::string icon;
            std::string pwd;
            std::string description;
        };

        // non-roaming app data
        std::string get_local_app_data_path();

        // roaming app data
        std::string get_app_data_folder();

        void exec(const std::string& path, const std::string& parameters);

        void set_rounded_corners(HWND hWnd, int radius = 20);

        void send_wm_copydata(HWND hWnd, const std::string& data, long data_type = 1);

        std::string get_wm_copydata_data(WPARAM wParam, LPARAM lParam, long& data_type);

        void open_mssettings(const std::string name);

        void open_default_apps();

        shell_link read_link(const std::string& path);

        unsigned int get_dpi();

        unsigned int get_dpi(HWND hWnd);

        std::string file_open_dialog(const std::string& file_type_name, const std::string& extension);
    }
}