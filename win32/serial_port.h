#pragma once
#include <string>
#include <vector>
#include <Windows.h>

namespace win32 {
    class serial_port {
    public:
        serial_port(const std::string& name, const std::string& friendly_name,
            DWORD baud_rate = CBR_9600,
            BYTE byte_size = 8,
            BYTE stop_bits = ONESTOPBIT,
            BYTE parity = NOPARITY)
            : name{name}, friendly_name{friendly_name},
            baud_rate{baud_rate}, byte_size{byte_size}, stop_bits{stop_bits}, parity{parity} {
        }
        ~serial_port() { close(); }

        static std::vector<serial_port> enumerate();

        const std::string name;
        const std::string friendly_name;

        bool send(const std::string& data);
        bool recv(std::string& data, size_t size);

    private:
        bool is_open{false};
        HANDLE hSerial{0};
        DWORD baud_rate;
        BYTE byte_size;
        BYTE stop_bits;
        BYTE parity;

        void open();
        void close();
    };
}