#include "github.h"
#include <fmt/core.h>

using namespace std;

namespace ext {

    std::string github::get_latest_release(const std::string& author, const std::string& repo) {
        string json = h.get("api.github.com", fmt::format("repos/{}/{}/releases/latest", author, repo));

        return "";
    }
}