#pragma once
#include <string>
#include <vector>
#include <sstream>

namespace str
{
   std::wstring to_wstr(const std::string& str);

   std::string to_str(const std::wstring& wstr);

   int to_int(const std::string& str);

   void ltrim(std::string& s, const std::string& chars = " \r\n\t");

   void rtrim(std::string& s, const std::string& chars = " \r\n\t");

   void trim(std::string& s, const std::string& chars = " \r\n\t");

   void upper(std::string& s);

   void lower(std::string& s);

   void unescape_special_chars(std::string& s);

   void replace_all(std::string& s, const std::string& search, const std::string& replacement);

   std::vector<std::string> split(const std::string& str, const std::string& delimiter);

   template<class ForwardIterator>
   std::string join(ForwardIterator begin, ForwardIterator end, const std::string& separator)
   {
      std::ostringstream ss;
      size_t count{};

      for (ForwardIterator it = begin; it != end; ++it, ++count)
      {
         if (count != 0) ss << separator;

         ss << *it;
      }

      return ss.str();
   }
   std::string to_human_readable_size(unsigned long size);

   std::vector<std::string> match_all_regex(const std::string& expression, const std::string& input);

   std::string get_domain_from_url(const std::string& url);

   std::string humanise(int value, std::string singular, std::string plural = "", std::string once = "", std::string twice = "");
}