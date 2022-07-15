#pragma once
#include <string>

namespace win32
{
   class kernel
   {
   public:
      static std::string get_computer_name();

      static std::string get_user_name();

      static std::string get_os_version();

      static int create_process(const std::string& cmdline,
         std::string& std_out,
         std::string& std_err);
   };
}