#pragma once
#include <string>
#include <vector>

namespace win32 {
    class serial_port {
    public:
        serial_port(const std::string& name, const std::string& friendly_name) : name{name}, friendly_name{friendly_name} {}

        static std::vector<serial_port> enumerate();

        const std::string name;
        const std::string friendly_name;
    };
}