#pragma once
#include <string>
#include <sstream>

namespace str
{
   std::wstring to_wstr(const std::string& str);

   std::string to_str(const std::wstring& wstr);

   void ltrim(std::string& s, unsigned char ch = ' ');

   void rtrim(std::string& s, unsigned char ch = ' ');

   void trim(std::string& s, unsigned char ch = ' ');

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

   std::string get_domain_from_url(const std::string& url);



}