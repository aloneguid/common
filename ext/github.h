#pragma once
#include <string>
#include "../win32/http.h"

namespace ext {

    class github_release {
    public:
        bool is_valid;
        std::string name;
        std::string tag;
        std::string home_url;
    };

    class github {
    public:
        github_release get_latest_release(const std::string& author, const std::string& repo);

    private:
        win32::http h;
    };
}