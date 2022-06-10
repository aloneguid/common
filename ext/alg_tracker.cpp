#include "alg_tracker.h"
#include "../str.h"
#include "fmt/core.h"

using namespace std;

namespace alg
{
   tracker::tracker(const std::string& app_name, const std::string& version) : version{ version }
   {
      url = "/events?key=";
      url += app_name;
   }

   void tracker::track(std::map<std::string, std::string> props)
   {
      string body;

      props["version"] = version;

      for (const auto& pair : props)
      {
         if (!pair.first.empty())
         {
            string value = pair.second;
            str::replace_all(value, "\\", "\\\\");

            if (!body.empty()) body += ",";
            body += fmt::format("\"{}\": \"{}\"", pair.first, value);
         }
      }

      body = string("{") + body + "}";


      h.post("alt.aloneguid.uk", url, body);
   }
}