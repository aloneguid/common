#include "http.h"
#include <vector>
#include <iterator>
#include "../../common/str.h"

#pragma comment(lib, "winhttp.lib")

using namespace std;

namespace win32
{

   http::http()
   {
      hSession = ::WinHttpOpen(
         L"ALG/1.0",
         WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
         WINHTTP_NO_PROXY_NAME,
         WINHTTP_NO_PROXY_BYPASS, 0);
   }

   http::~http()
   {
      if (hSession)
      {
         ::WinHttpCloseHandle(hSession);
      }
   }

   std::string http::get(const std::string& domain, const std::string& url) const
   {
      string result;

      if (!hSession) return result;

      HINTERNET hConnect = ::WinHttpConnect(hSession,
         str::to_wstr(domain).c_str(),
         INTERNET_DEFAULT_HTTPS_PORT, 0);

      if (hConnect)
      {
         wstring wurl{ str::to_wstr(url) };
         HINTERNET hRequest = ::WinHttpOpenRequest(hConnect,
            L"GET",
            wurl.c_str(),
            nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

         if (hRequest)
         {
            //wstring headers{ L"Content-Type: application/json" };
            bool ok = ::WinHttpSendRequest(hRequest,
               WINHTTP_NO_ADDITIONAL_HEADERS,
               0, WINHTTP_NO_REQUEST_DATA, 0,
               0, 0);

            ok = ::WinHttpReceiveResponse(hRequest, NULL);
            if (ok)
            {
               vector<char> buffer;
               buffer.resize(1024);
               DWORD read{ 0 };
               while (::WinHttpReadData(hRequest, &buffer[0], 1024, &read) && read > 0)
               {
                  std::copy(buffer.begin(), buffer.begin() + read, back_inserter(result));
               }
            }

            ::WinHttpCloseHandle(hRequest);
         }


         ::WinHttpCloseHandle(hConnect);
      }

      return result;
   }

   void http::post(const std::string& domain, const std::string& url, const std::string& data, bool is_async) const
   {
      // PUT example - https://docs.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpreceiveresponse?f1url=%3FappId%3DDev16IDEF1%26l%3DEN-US%26k%3Dk(WINHTTP%252FWinHttpReceiveResponse);k(WinHttpReceiveResponse);k(DevLang-C%252B%252B);k(TargetOS-Windows)%26rd%3Dtrue

      if (!hSession) return;

      HINTERNET hConnect = ::WinHttpConnect(hSession,
         str::to_wstr(domain).c_str(),
         INTERNET_DEFAULT_HTTPS_PORT, 0);

      if (hConnect)
      {
         wstring wurl{ str::to_wstr(url) };
         HINTERNET hRequest = ::WinHttpOpenRequest(hConnect,
            L"POST",
            wurl.c_str(),
            nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

         // --------- SYNC/ASYNC ----------
         // from here commonality between sync and async ends
         // https://docs.microsoft.com/en-us/archive/msdn-magazine/2008/august/windows-with-c-asynchronous-winhttp

         if (hRequest)
         {
            //wstring headers{ L"Content-Type: application/json" };
            bool ok = ::WinHttpSendRequest(hRequest,
               WINHTTP_NO_ADDITIONAL_HEADERS,
               0, WINHTTP_NO_REQUEST_DATA, 0,
               data.size(), 0);

            DWORD dwBytesWritten{ 0 };
            ok = ::WinHttpWriteData(hRequest, (LPCVOID)&data[0], data.size(), &dwBytesWritten);
            ok = ::WinHttpReceiveResponse(hRequest, NULL);
         }

         ::WinHttpCloseHandle(hRequest);
      }

      ::WinHttpCloseHandle(hConnect);
   }
}