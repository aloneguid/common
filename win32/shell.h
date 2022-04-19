#include <string>
#include <windows.h>

namespace win32
{
   namespace shell
   {
      std::string get_local_app_data_path();

      void exec(const std::string& path, const std::string& parameters);

      void set_rounded_corners(HWND hWnd, int radius = 20);

      void send_wm_copydata(HWND hWnd, const std::string& data, long data_type = 1);

      std::string get_wm_copydata_data(WPARAM wParam, LPARAM lParam, long& data_type);

      void open_mssettings(const std::string name);

      void open_default_apps();

      void add_shell_notify_icon();

      void remove_shell_notify_icon();
   }
}

