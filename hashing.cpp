#include "hashing.h"
#if WIN32
#include <Windows.h>
#include <bcrypt.h>
#endif
#include <sstream>
#include <iomanip>

#if WIN32
#pragma comment(lib, "bcrypt.lib")
#endif

namespace hashing {

#if WIN32
    std::string md5(const std::string& input) {
        BCRYPT_ALG_HANDLE alg = nullptr;
        BCRYPT_HASH_HANDLE hash_handle = nullptr;
        std::string result;

        if(BCryptOpenAlgorithmProvider(&alg, BCRYPT_MD5_ALGORITHM, nullptr, 0) != 0) {
            return result;
        }

        if(BCryptCreateHash(alg, &hash_handle, nullptr, 0, nullptr, 0, 0) != 0) {
            BCryptCloseAlgorithmProvider(alg, 0);
            return result;
        }

        if(BCryptHashData(hash_handle,
            reinterpret_cast<PUCHAR>(const_cast<char*>(input.data())),
            static_cast<ULONG>(input.size()), 0) != 0) {
            BCryptDestroyHash(hash_handle);
            BCryptCloseAlgorithmProvider(alg, 0);
            return result;
        }

        UCHAR digest[16]; // MD5 is always 16 bytes
        if(BCryptFinishHash(hash_handle, digest, sizeof(digest), 0) != 0) {
            BCryptDestroyHash(hash_handle);
            BCryptCloseAlgorithmProvider(alg, 0);
            return result;
        }

        BCryptDestroyHash(hash_handle);
        BCryptCloseAlgorithmProvider(alg, 0);

        // convert to hex string
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for(int i = 0; i < 16; ++i) {
            oss << std::setw(2) << static_cast<int>(digest[i]);
        }
        result = oss.str();

        return result;
    }
#endif
}