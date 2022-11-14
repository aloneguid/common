#pragma once
#include <string>

namespace fss
{
   std::string get_current_dir();

   std::string get_current_exec_path();

   bool file_exists(const std::string& name);

   size_t get_file_size(const std::string& name);

   bool read_binary_file(const std::string& name, unsigned char* buffer);

   /// <summary>
   /// Reads file as string, returns empty string on error.
   /// </summary>
   std::string read_file_as_string(const std::string& name);

   void write_file_as_string(const std::string& filename, const std::string& content);
}