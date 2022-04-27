#include <string>
#include <vector>
#include <windows.h>
#include <cmath>
#include <sstream>
#include <regex>

using namespace std;

namespace str
{
   std::wstring to_wstr(const std::string& str)
   {
      if (str.empty()) return std::wstring();
      int size_needed = ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
      std::wstring wstrTo(size_needed, 0);
      ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
      return wstrTo;
   }

   std::string to_str(const std::wstring& wstr)
   {
      if (wstr.empty()) return std::string();
      int size_needed = ::WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
      std::string strTo(size_needed, 0);
      ::WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
      return strTo;
   }

   // trim from start (in place)
   void ltrim(std::string& s, unsigned char ch = ' ')
   {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [ch](unsigned char ch1)
                                      {
                                         return ch != ch1;
                                      }));
   }

   // trim from end (in place)
   void rtrim(std::string& s, unsigned char ch = ' ')
   {
      s.erase(std::find_if(s.rbegin(), s.rend(), [ch](unsigned char ch1)
                           {
                              return ch != ch1;
                           }).base(), s.end());
   }

   void trim(std::string& s, unsigned char ch = ' ')
   {
      ltrim(s, ch);
      rtrim(s, ch);
   }


   /// <summary>
   /// Replaces (in-place) all occurances of target with replacement. Taken from : http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string.
   /// </summary>
   /// <param name="str"></param>
   /// <param name="target"></param>
   /// <param name="replecament"></param>
   /// <returns></returns>
   template<class T>
   bool replace_all(std::basic_string<T>& str, const std::basic_string<T>& target, const std::basic_string<T>& replacement)
   {
      if (target.empty())
      {
         return false;
      }

      size_t start_pos = 0;
      const bool found_substring = str.find(target, start_pos) != std::string::npos;

      while ((start_pos = str.find(target, start_pos)) != std::string::npos)
      {
         str.replace(start_pos, target.length(), replacement);
         start_pos += replacement.length();
      }

      return found_substring;
   }

   std::vector<std::string> split(const std::string& str,
                                                const std::string& delimiter)
   {
       std::vector<std::string> strings;

       std::string::size_type pos = 0;
       std::string::size_type prev = 0;
       while ((pos = str.find(delimiter, prev)) != std::string::npos)
       {
           strings.push_back(str.substr(prev, pos - prev));
           prev = pos + 1;
       }

       // To get the last substring (or only, if delimiter is not found)
       strings.push_back(str.substr(prev));

       return strings;
   }

   std::string to_human_readable_size(unsigned long size)
   {
      int i{};
      double mantissa = size;
      for (; mantissa >= 1024.; mantissa /= 1024., ++i) {}

      //mantissa = std::ceil(mantissa * 10.) / 10.;
      //return std::to_string(mantissa) + "BKMGTPE"[i];

      ostringstream out;
      out.precision(2);
      out << std::fixed << mantissa;
      return out.str() + "BKMGTPE"[i];
   }

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