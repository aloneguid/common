#pragma once
#include <string>

namespace win32
{
   class kernel
   {
   public:

      static std::string get_last_error_text();

      static std::string get_computer_name();

      static std::string get_user_name();

      static int create_process(const std::string& cmdline,
         std::string& std_out,
         std::string& std_err,
         std::string& sys_error_text);
   };
}