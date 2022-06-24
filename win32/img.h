#pragma once
#include <string>

namespace win32
{
   class img
   {
   public:
      static void load_from_file(const std::string& path);
   };
}