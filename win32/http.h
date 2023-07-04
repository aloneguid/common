#pragma once
#include <string>
#include <map>
#include <windows.h>
#include <winhttp.h>

namespace win32
{
   /// <summary>
   /// Uses WinHTTP. This is useful if you want as little dependencies as possible in your executable.
   /// </summary>
   class http
   {
   public:
      http();
      ~http();

      std::string get(const std::string& domain, const std::string& url) const;

      int get_get_headers(const std::string& url, std::map<std::string, std::string>& headers) const;

      void post(const std::string& domain, const std::string& url, const std::string& data, bool is_async = false) const;

   private:
      HINTERNET hSession{ 0 };

      bool disable_redirects(HINTERNET hRequest) const;
   };
}