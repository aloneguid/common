#include "github.h"
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "str.h"

using namespace std;
using json = nlohmann::json;

namespace ext {

    github_release github::get_latest_release(const std::string& author, const std::string& repo) {
        string js = h.get(fmt::format("https://api.github.com/repos/{}/{}/releases/latest", author, repo));

        try {
            auto j = json::parse(js);

            string name = j["name"].get<string>();
            string tag = j["tag_name"].get<string>();
            string url = j["html_url"].get<string>();
            str::trim(name);
            str::trim(tag);
            str::trim(url);

            return github_release{true, name, tag, url};
        } catch(json::type_error) {
            return github_release{false};
        }
    }
}