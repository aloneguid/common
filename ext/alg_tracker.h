#pragma once
#include <string>
#include <map>
#include <vector>
#include "../win32/http.h"

namespace alg
{
   class tracker
   {
   public:
      tracker(const std::string& app_name, const std::string& version);

      void track(const std::map<std::string, std::string>& props, bool flush_now);

      void flush();

   private:
      win32::http h;
      std::string url;
      std::string version;
      std::vector<std::string> queue;
   };
}