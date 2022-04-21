#include "web.h"
#include <regex>

using namespace std;

namespace web
{
   std::string get_domain_from_url(const std::string& url)
   {
      regex r("https?:\\/\\/(www\\.)?([-a-zA-Z0-9@:%._\\+~#=]{1,256})");
      smatch sm;
      // group 2 is the domain
      if (regex_search(url, sm, r) && sm.size() == 3)
      {
         return sm[2].str();
      }

      return "";
   }
}