#include "serial_port.h"
#include <initguid.h>
#include <SetupAPI.h>
#include <devpkey.h>
#include "str.h"
#include "reg.h"
#include "kernel.h"

#pragma comment(lib, "setupapi.lib")

namespace win32 {

    using namespace std;

    string get_device_registry_property(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA* devInfo, DWORD property) {
        DWORD dwType{0};
        DWORD dwSize{0};
        //q uery initially to get the buffer size required
        ::SetupDiGetDeviceRegistryPropertyW(hDevInfoSet, devInfo, property, &dwType, nullptr, 0, &dwSize);

        std::vector<BYTE> value{dwSize, std::allocator<BYTE>{}}; //allocate buffer
        if(SetupDiGetDeviceRegistryPropertyW(hDevInfoSet, devInfo, property, &dwType, value.data(), dwSize, &dwSize)) {
            return str::to_str(reinterpret_cast<wchar_t*>(value.data()));
        }

    }

    std::string get_device_property(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA* devInfo, const DEVPROPKEY* property_key) {
        DEVPROPTYPE propType;
        WCHAR buffer[256];
        DWORD requiredSize = 0;
        if(SetupDiGetDevicePropertyW(
            hDevInfoSet,
            devInfo,
            property_key,
            &propType,
            (PBYTE)buffer,
            sizeof(buffer),
            &requiredSize,
            0)) {
            return str::to_str(buffer);
        }
        return "";
    }

    bool QueryUsingSetupAPI(const GUID& guid, _In_ DWORD dwFlags, vector<serial_port>& r) {

        // create a "device information set" for the specified GUID
        HDEVINFO hDevInfoSet{SetupDiGetClassDevs(&guid, nullptr, nullptr, dwFlags)};
        if(hDevInfoSet == INVALID_HANDLE_VALUE)
            return false;

        // do the enumeration
        bool has_more{true};
        int idx{0};
        SP_DEVINFO_DATA devInfo{};
        while(has_more) {
            //Enumerate the current device
            devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
            has_more = SetupDiEnumDeviceInfo(hDevInfoSet, idx, &devInfo);
            if(has_more) {
                string name;

                // get the registry key which stores the ports settings in order to get port name (COM1, COM2, etc)
                HKEY deviceKey = ::SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
                if(deviceKey != INVALID_HANDLE_VALUE) {
                    name = reg::get_value(deviceKey, "PortName");
                    ::RegCloseKey(deviceKey);
                }

                // get the friendly name of the device (human readable name)
                string friendly_name = get_device_registry_property(hDevInfoSet, &devInfo, SPDRP_FRIENDLYNAME);
                string description = get_device_property(hDevInfoSet, &devInfo, &DEVPKEY_Device_BusReportedDeviceDesc);

                if(!name.empty()) {
                    r.emplace_back(name, friendly_name, description);
                }

            }

            ++idx;
        }

        //Free up the "device information set" now that we are finished with it
        SetupDiDestroyDeviceInfoList(hDevInfoSet);

        //Return the success indicator
        return true;
    }

    std::vector<serial_port> serial_port::enumerate(DWORD default_baud_rate) {
        vector<serial_port> r;
        QueryUsingSetupAPI(GUID_DEVINTERFACE_COMPORT, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, r);

        for(serial_port& port : r) {
            port.baud_rate = default_baud_rate; // set default baud rate
        }

        return r;
    }

    bool serial_port::send(const std::string& data) {
        return send(data.data(), data.size());
    }

    bool serial_port::send(const char* data, size_t size) {
        if(!is_open) open();
        if(!is_open) {
            return false;
        }

        // send the data
        DWORD dwBytesWritten{0};
        bool ok = WriteFile(hSerial, data, size, &dwBytesWritten, nullptr);
        if(!ok) {
            // handle error
            throw std::exception(("write failed: " + win32::kernel::get_last_error_text()).c_str());
        }
        return ok;
    }

    bool serial_port::send(const std::vector<uint8_t> data) {
        return send((const char*)data.data(), data.size());
    }

    bool serial_port::recv(std::string& data, size_t size) {

        if(!is_open) open();
        if(!is_open) return false;

        std::vector<char> buffer(size);
        DWORD dwBytesRead{0};
        bool ok = ReadFile(hSerial, buffer.data(), size, &dwBytesRead, nullptr);
        if(ok) {
            data.assign(buffer.begin(), buffer.begin() + dwBytesRead);
        }
        return ok;
    }

    void serial_port::purge() {
        ::PurgeComm(hSerial, PURGE_RXCLEAR);
        ::PurgeComm(hSerial, PURGE_TXCLEAR);
    }

    void serial_port::open() {

        // "COM10" and above: Must use "\\\\.\\COM10", "\\\\.\\COM14", etc.

        string port_filename = name;
        if(!port_filename.starts_with("\\\\.\\")) {
            port_filename = "\\\\.\\" + port_filename;
        }
        wstring w_name = str::to_wstr(port_filename);

        hSerial = ::CreateFile(w_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if(hSerial == INVALID_HANDLE_VALUE) {
            throw std::exception(("can't open port: " + win32::kernel::get_last_error_text()).c_str());
        } else {
            // set the COM port settings
            DCB dcbSerialParams = {0};
            dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
            if(!GetCommState(hSerial, &dcbSerialParams)) {
                //error getting state
                ::CloseHandle(hSerial);
                hSerial = INVALID_HANDLE_VALUE;
            } else {
                dcbSerialParams.BaudRate = baud_rate;
                dcbSerialParams.ByteSize = byte_size;
                dcbSerialParams.StopBits = stop_bits;
                dcbSerialParams.Parity = parity;
                dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE; // enable DTR
                if(!SetCommState(hSerial, &dcbSerialParams)) {
                    //error setting serial port state
                    ::CloseHandle(hSerial);
                    hSerial = INVALID_HANDLE_VALUE;
                }
            }

            if(hSerial == INVALID_HANDLE_VALUE) {
                return;
            }

            // set timeouts
            COMMTIMEOUTS timeouts = {0};

            timeouts.ReadIntervalTimeout = 50;
            timeouts.ReadTotalTimeoutConstant = 50;
            timeouts.ReadTotalTimeoutMultiplier = 10;

            //timeouts.ReadIntervalTimeout = MAXDWORD;
            //timeouts.ReadTotalTimeoutMultiplier = 0;
            //timeouts.ReadTotalTimeoutConstant = 0;

            timeouts.WriteTotalTimeoutConstant = 500;
            timeouts.WriteTotalTimeoutMultiplier = 10;
            if(!SetCommTimeouts(hSerial, &timeouts)) {
                //error setting timeouts
                ::CloseHandle(hSerial);
            }

            is_open = true;
        }
    }

    void serial_port::close() {
        if(is_open) {
            ::CloseHandle(hSerial);
            is_open = false;
        }
    }
}