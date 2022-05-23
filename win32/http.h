#pragma once
#include <string>
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

      std::string get(const std::string& domain, const std::string& url);

      void post(const std::string& domain, const std::string& url, const std::string& data, bool is_async = false);

   private:
      HINTERNET hSession{ 0 };
   };
}