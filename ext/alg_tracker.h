#pragma once
#include <string>
#include <map>
#include <vector>
#include "../win32/http.h"

namespace alg
{
    class tracker {
    public:
        tracker(const std::string& app_name, const std::string& version,
            const std::string& cache_file_path = "", unsigned int max_cache_age_hours = 1);
        ~tracker();

        void track(const std::map<std::string, std::string>& props);

        void add_constant(std::string name, std::string value);

        void flush();

    private:
        win32::http h;
        const std::string url;
        std::string version;
        std::map<std::string, std::string> constants;
        const std::string cache_file_path;
        unsigned int max_cache_age_hours;

        bool write_to_file(const std::string& body);
    };
}