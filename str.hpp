#include <string>
#include <windows.h>

namespace str
{
   static std::wstring to_wstr(const std::string& str)
   {
      if (str.empty()) return std::wstring();
      int size_needed = ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
      std::wstring wstrTo(size_needed, 0);
      ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
      return wstrTo;
   }

   static std::string to_str(const std::wstring& wstr)
   {
      if (wstr.empty()) return std::string();
      int size_needed = ::WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
      std::string strTo(size_needed, 0);
      ::WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
      return strTo;
   }

   // trim from start (in place)
   static inline void ltrim(std::string& s)
   {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                      {
                                         return !std::isspace(ch);
                                      }));
   }

   // trim from end (in place)
   static inline void rtrim(std::string& s)
   {
      s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                           {
                              return !std::isspace(ch);
                           }).base(), s.end());
   }


   /// <summary>
   /// Replaces (in-place) all occurances of target with replacement. Taken from : http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string.
   /// </summary>
   /// <param name="str"></param>
   /// <param name="target"></param>
   /// <param name="replecament"></param>
   /// <returns></returns>
   template<class T>
   static inline bool replace_all(std::basic_string<T>& str, const std::basic_string<T>& target, const std::basic_string<T>& replacement)
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

   static inline std::vector<std::string> split(const std::string& str,
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

   static inline std::string to_human_readable_size(unsigned long size)
   {
      int i{};
      double mantissa = size;
      for (; mantissa >= 1024.; mantissa /= 1024., ++i) {}
      mantissa = std::ceil(mantissa * 10.) / 10.;
      return std::to_string(mantissa) + "BKMGTPE"[i];
   }

}