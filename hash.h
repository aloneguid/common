#pragma once
#include <string>

namespace hash {
    static std::string md5(const std::string& s);
    static std::string sha256(const std::string& s);
    static std::string sha512(const std::string& s);
}