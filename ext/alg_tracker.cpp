#include "alg_tracker.h"
#include "../str.h"
#include "fmt/core.h"
#include "datetime.h"

using namespace std;

namespace alg
{
    tracker::tracker(const std::string& app_name, const std::string& version) : version{version} {
        url = "/events?key=";
        url += app_name;
    }

    tracker::~tracker() {
        flush();
    }

    void tracker::track(const std::map<std::string, std::string>& props1, bool flush_now) {
        string body;

        map<string, string> props = props1;
        props["t"] = datetime::to_iso_8601() + "Z";
        props["version"] = version;

        if(!constants.empty()) {
            for(const auto& pair : constants) {
                if(pair.first.empty() || pair.second.empty()) continue;
                props[pair.first] = pair.second;
            }
        }

        for(const auto& pair : props) {
            if(!pair.first.empty()) {
                string value = pair.second;
                str::replace_all(value, "\\", "\\\\");

                if(!body.empty()) body += ",";
                body += fmt::format("\"{}\": \"{}\"", pair.first, value);
            }
        }

        body = string("{") + body + "}";

        queue.push_back(body);

        if(flush_now) flush();
    }

    void tracker::add_constant(std::string name, std::string value) {
        constants[name] = value;
    }

    void tracker::flush() {

        if(queue.empty()) return;

        string body{"["};
        body += str::join(queue.begin(), queue.end(), ",");
        body += "]";

        h.post("alt.aloneguid.uk", url, body);

        queue.clear();
    }
}