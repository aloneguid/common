#pragma once
#include <string>
#include <windows.h>
#include <winhttp.h>

namespace win32
{
   class http
   {
   public:
      http();
      ~http();

      std::string get(const std::string& domain, const std::string& url);

      void post(const std::string& domain, const std::string& url, const std::string& data);

   private:
      HINTERNET hSession{ 0 };
   };
}