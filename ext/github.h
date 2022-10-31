#pragma once
#include <string>
#include "../win32/http.h"

namespace ext {
    class github {
    public:
        std::string get_latest_release(const std::string& author, const std::string& repo);

    private:
        win32::http h;
    };
}