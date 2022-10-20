#pragma once
#include <string>
#include <Windows.h>

namespace win32 {
    /**
     * @brief Uses Windows CNG (Crypto Next Gen) (to avoid heavy dependencies like OpenSSL)
    */
    class cng {
    public:
        static std::string md5(const std::string& s);
        static std::string sha256(const std::string& s);
        static std::string sha512(const std::string& s);

    private:
        static std::string hash(const std::string& s, LPCWSTR pszAlgId);
    };
}