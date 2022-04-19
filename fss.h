#pragma once
#include <string>

namespace fss
{
   std::string get_current_dir();

   std::string get_current_exec_path();

   size_t get_file_size(const std::string& name);

   bool read_binary_file(const std::string& name, unsigned char* buffer);

   std::string read_file_as_string(const std::string& name);
}