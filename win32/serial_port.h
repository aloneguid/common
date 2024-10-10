#pragma once
#include <string>
#include <vector>
#include <Windows.h>

namespace win32 {
    class serial_port {
    public:
        serial_port(const std::string& name, const std::string& friendly_name) : name{name}, friendly_name{friendly_name} {}
        ~serial_port() { close(); }

        static std::vector<serial_port> enumerate();

        const std::string name;
        const std::string friendly_name;

        bool send(const std::string& data);

    private:
        bool is_open{false};
        HANDLE hSerial{0};

        void open();
        void close();
    };
}