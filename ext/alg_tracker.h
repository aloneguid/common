#pragma once
#include <string>
#include <map>
#include "../win32/http.h"

namespace alg
{
   class tracker
   {
   public:
      tracker(const std::string& app_name, const std::string& version);

      void track(std::map<std::string, std::string> props);

   private:
      win32::http h;
      std::string url;
      std::string version;
   };
}