#include "github.h"
#include <fmt/core.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace ext {

    github_release github::get_latest_release(const std::string& author, const std::string& repo) {
        string js = h.get("api.github.com", fmt::format("repos/{}/{}/releases/latest", author, repo));

        auto j = json::parse(js);

        return github_release{ 
            j["name"].get<string>(),
            j["tag_name"].get<string>(),
            j["html_url"].get<string>()
        };
    }
}