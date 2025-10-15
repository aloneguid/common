#pragma once
#include <string>

namespace fss {
   std::string get_current_dir();

   std::string get_current_exec_path();

   bool file_exists(const std::string& name);

   size_t get_file_size(const std::string& name);

   std::string get_full_path(const std::string& path);

   bool read_binary_file(const std::string& name, unsigned char* buffer);

   /// <summary>
   /// Reads file as string, returns empty string on error.
   /// </summary>
   std::string read_file_as_string(const std::string& name);

   /**
    * @brief Writes all text to file, overwriting if exists.
    * @param filename 
    * @param contents 
    */
   void write_all_text(const std::string& filename, const std::string& contents);

   /**
    * @brief Writes all text to file, appending if exists.
    * @param filename
    * @param contents
    */
   void append_all_text(const std::string& filename, const std::string& contents);

   /**
    * @brief Calculates file age in seconds, from the creation time.
    * @param filename 
    * @return 
    */
   unsigned int get_age_in_seconds(const std::string& filename);

   std::string get_temp_file_path(const std::string& prefix = "TMP");

   bool delete_file(const std::string& path);
}