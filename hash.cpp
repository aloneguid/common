#include "hash.h"
#if _WIN32
#include <Windows.h>

namespace hash {

    using namespace std;

#if _WIN32
    // Uses Windows CNG(Crypto Next Gen) (to avoid heavy dependencies like OpenSSL)
    std::string win32_cng_hash(const std::string& s, LPCWSTR pszAlgId) {
        string r;

        BCRYPT_ALG_HANDLE hAlg = NULL;
        DWORD cbHashObject{0};
        DWORD cbData{0};
        PBYTE pbHashObject{NULL};
        BCRYPT_HASH_HANDLE hHash{NULL};
        PBYTE pbHash{NULL};
        DWORD cbHash{0};
        NTSTATUS status = STATUS_UNSUCCESSFUL;

        //open an algorithm handle
        if(NT_SUCCESS(::BCryptOpenAlgorithmProvider(
            &hAlg,
            pszAlgId,
            NULL,
            0))) {

            //calculate the size of the buffer to hold the hash object
            if(NT_SUCCESS(::BCryptGetProperty(
                hAlg,
                BCRYPT_OBJECT_LENGTH,
                (PBYTE)&cbHashObject,
                sizeof(DWORD),
                &cbData,
                0))) {

                //allocate the hash object on the heap
                pbHashObject = (PBYTE)::HeapAlloc(::GetProcessHeap(), 0, cbHashObject);
                if(pbHashObject) {

                    // calculate the length of the hash
                    if(NT_SUCCESS(::BCryptGetProperty(
                        hAlg,
                        BCRYPT_HASH_LENGTH,
                        (PBYTE)&cbHash,
                        sizeof(DWORD),
                        &cbData,
                        0))) {

                        //allocate the hash buffer on the heap
                        pbHash = (PBYTE)::HeapAlloc(::GetProcessHeap(), 0, cbHash);
                        if(pbHash) {

                            //create a hash
                            if(NT_SUCCESS(status = ::BCryptCreateHash(
                                hAlg,
                                &hHash,
                                pbHashObject,
                                cbHashObject,
                                NULL,
                                0,
                                0))) {

                                //hash some data
                                if(NT_SUCCESS(::BCryptHashData(
                                    hHash,
                                    (PBYTE)&s[0],
                                    s.size(),
                                    0))) {

                                    // close the hash
                                    if(NT_SUCCESS(::BCryptFinishHash(
                                        hHash,
                                        pbHash,
                                        cbHash,
                                        0))) {

                                        // fit into safe C++
                                        r.resize(cbHash * 2);
                                        for(unsigned int i = 0; i < cbHash; ++i)
                                            std::sprintf(&r[i * 2], "%02X", pbHash[i]);

                                        ::HeapFree(::GetProcessHeap(), 0, pbHash);
                                    }

                                }
                                ::BCryptDestroyHash(hHash);
                            }
                        }
                    }

                    ::HeapFree(::GetProcessHeap(), 0, pbHashObject);
                }
            }

            ::BCryptCloseAlgorithmProvider(hAlg, 0);
        }

        return r;
    }
#endif

    std::string md5(const std::string& s) {
#if _WIN32
        return hash(s, BCRYPT_MD5_ALGORITHM);
#else
        return "";
#endif
    }

    std::string sha256(const std::string& s) {
#if _WIN32
        return hash(s, BCRYPT_SHA256_ALGORITHM);
#else
        return "";
#endif
    }

    std::string sha512(const std::string& s) {
#if _WIN32
        return hash(s, BCRYPT_SHA512_ALGORITHM);
#else
        return "";
#endif
    }
}