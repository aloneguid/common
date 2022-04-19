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

   /*template<class T>
   static inline bool contains(const std::basic_string<T>& haystack, const std::basic_string<T>& needle)
   {
       return haystack.find(needle) != std::string::npos;
   }

   template<class T>
   static inline std::basic_string<T> lower(const std::basic_string<T>& str)
   {
       basic_string<T> result(str);
       std::transform(result.begin(), result.end(), result.begin(), ::lower);
   }

   template<class T>
   static inline bool ci_contains(const std::basic_string<T>& haystack, const std::basic_string<T>& needle)
   {
       return lower(haystack).find(lower(needle)) != std::string::npos;
   }*/


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

   template<class T>
   static bool test(std::basic_string<T> s)
   {
      return s.empty();
   }
}