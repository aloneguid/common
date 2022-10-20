#include "cng.h"
#include <Windows.h>
#include <bcrypt.h>
#include <vector>

#pragma comment(lib, "bcrypt.lib")

#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL         ((NTSTATUS)0xC0000001L)

using namespace std;

namespace win32 {

    // see https://learn.microsoft.com/en-us/windows/win32/seccng/creating-a-hash-with-cng


    std::string cng::md5(const std::string& s) {
        return hash(s, BCRYPT_MD5_ALGORITHM);
    }

    std::string cng::sha256(const std::string& s) {
        return hash(s, BCRYPT_SHA256_ALGORITHM);
    }

    std::string cng::sha512(const std::string& s) {
        return hash(s, BCRYPT_SHA512_ALGORITHM);
    }

    std::string cng::hash(const std::string& s, LPCWSTR pszAlgId) {
        string r;

        BCRYPT_ALG_HANDLE hAlg = NULL;
        DWORD cbHashObject{ 0 };
        DWORD cbData{ 0 };
        PBYTE pbHashObject{ NULL };
        BCRYPT_HASH_HANDLE hHash{ NULL };
        PBYTE pbHash{ NULL };
        DWORD cbHash{ 0 };
        NTSTATUS status = STATUS_UNSUCCESSFUL;

        //open an algorithm handle
        if (NT_SUCCESS(::BCryptOpenAlgorithmProvider(
            &hAlg,
            pszAlgId,
            NULL,
            0))) {

            //calculate the size of the buffer to hold the hash object
            if (NT_SUCCESS(::BCryptGetProperty(
                hAlg,
                BCRYPT_OBJECT_LENGTH,
                (PBYTE)&cbHashObject,
                sizeof(DWORD),
                &cbData,
                0))) {

                //allocate the hash object on the heap
                pbHashObject = (PBYTE)::HeapAlloc(::GetProcessHeap(), 0, cbHashObject);
                if (pbHashObject) {

                    // calculate the length of the hash
                    if (NT_SUCCESS(::BCryptGetProperty(
                        hAlg,
                        BCRYPT_HASH_LENGTH,
                        (PBYTE)&cbHash,
                        sizeof(DWORD),
                        &cbData,
                        0))) {

                        //allocate the hash buffer on the heap
                        pbHash = (PBYTE)::HeapAlloc(::GetProcessHeap(), 0, cbHash);
                        if (pbHash) {

                            //create a hash
                            if (NT_SUCCESS(status = ::BCryptCreateHash(
                                hAlg,
                                &hHash,
                                pbHashObject,
                                cbHashObject,
                                NULL,
                                0,
                                0))) {

                                //hash some data
                                if (NT_SUCCESS(::BCryptHashData(
                                    hHash,
                                    (PBYTE)&s[0],
                                    s.size(),
                                    0))) {

                                    // close the hash
                                    if (NT_SUCCESS(::BCryptFinishHash(
                                        hHash,
                                        pbHash,
                                        cbHash,
                                        0))) {

                                        // fit into safe C++
                                        r.resize(cbHash * 2);
                                        for (unsigned int i = 0; i < cbHash; ++i)
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
}