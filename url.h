#pragma once
#include <string>
#include <map>

class url {
public:
    url(const std::string& abs_url);

    std::string abs_url;
    std::string protocol;
    std::string host;
    std::string query;

    std::map<std::string, std::string> parameters;
};