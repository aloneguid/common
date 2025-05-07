#include "alg_tracker.h"
#include "../str.h"
#include "fmt/core.h"
#include "datetime.h"
#include "../fss.h"

using namespace std;

namespace alg
{
    const string DomainName = "alt2.aloneguid.uk";

    tracker::tracker(const std::string& app_name, const std::string& version,
        const std::string& cache_file_path, unsigned int max_cache_age_hours) 
        : version{version}, url{"/events?key=" + app_name},
        cache_file_path{cache_file_path}, max_cache_age_hours{max_cache_age_hours} {

        flush();
    }

    tracker::~tracker() {
        flush();
    }

    void tracker::track(const std::map<std::string, std::string>& props1) {
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

                if(!body.empty()) body += ", ";
                body += fmt::format("\"{}\": \"{}\"", pair.first, value);
            }
        }

        body = string("{") + body + "}";
        write_to_file(body);
    }

    void tracker::add_constant(std::string name, std::string value) {
        constants[name] = value;
    }

    void tracker::flush() {
        auto age = fss::get_age_in_seconds(cache_file_path);

        if(age < max_cache_age_hours * 3600) {
            return;
        }

        // read all file contents
        string all_text = fss::read_file_as_string(cache_file_path);
        fss::delete_file(cache_file_path);

        // split by new line
        auto lines1 = str::split(all_text, "\n");

        // delete empty lines
        vector<string> lines;
        for(auto& line : lines1) {
            string l = line;
            str::trim(l);
            if(l.empty()) continue;
            lines.push_back(l);
        }

        if(lines.empty()) return;


        string body{"["};
        body += str::join(lines.begin(), lines.end(), ", ");
        body += "]";

        h.post(DomainName, url, body);
    }

    bool alg::tracker::write_to_file(const std::string& body) {
        fss::append_all_text(cache_file_path, body + "\n");
        return true;
    }
}