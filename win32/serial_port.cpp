#include "serial_port.h"
#include <SetupAPI.h>
#include "str.h"
#include "reg.h"


#pragma comment(lib, "setupapi.lib")

namespace win32 {

    using namespace std;

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
                string friendly_name;

                // get the registry key which stores the ports settings in order to get port name (COM1, COM2, etc)
                HKEY deviceKey = ::SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
                if(deviceKey != INVALID_HANDLE_VALUE) {
                    name = reg::get_value(deviceKey, "PortName");
                    ::RegCloseKey(deviceKey);
                }

                // get the friendly name of the device (human readable name)
                {
                    DWORD dwType{0};
                    DWORD dwSize{0};
                    //q uery initially to get the buffer size required
                    ::SetupDiGetDeviceRegistryPropertyW(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, nullptr, 0, &dwSize);

                    std::vector<BYTE> friendlyName{dwSize, std::allocator<BYTE>{}}; //allocate buffer
                    if(SetupDiGetDeviceRegistryPropertyW(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, friendlyName.data(), dwSize, &dwSize)) {
                        friendly_name = str::to_str(reinterpret_cast<wchar_t*>(friendlyName.data()));
                    }
                }

                if(!name.empty()) {
                    r.emplace_back(name, friendly_name);
                }

            }

            ++idx;
        }

        //Free up the "device information set" now that we are finished with it
        SetupDiDestroyDeviceInfoList(hDevInfoSet);

        //Return the success indicator
        return true;
    }

    std::vector<serial_port> serial_port::enumerate() {
        vector<serial_port> r;
        QueryUsingSetupAPI(GUID_DEVINTERFACE_COMPORT, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, r);
        return r;
    }

    bool serial_port::send(const std::string& data) {
        if(!is_open) open();
        if(!is_open) return false;

        // send the data
        DWORD dwBytesWritten{0};
        bool ok = WriteFile(hSerial, data.c_str(), data.size(), &dwBytesWritten, nullptr);
        return ok;
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

    void serial_port::open() {
        wstring w_name = str::to_wstr(name);
        hSerial = ::CreateFile(w_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if(hSerial == INVALID_HANDLE_VALUE) {
            DWORD dwError{GetLastError()};
            if(dwError == ERROR_FILE_NOT_FOUND) {
                //serial port does not exist. Inform user.
            } else {
                //some other error occurred. Inform user.
            }
        } else {
            // set the COM port settings
            DCB dcbSerialParams = {0};
            dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
            if(!GetCommState(hSerial, &dcbSerialParams)) {
                //error getting state
                ::CloseHandle(hSerial);
            } else {
                dcbSerialParams.BaudRate = CBR_9600;
                dcbSerialParams.ByteSize = 8;
                dcbSerialParams.StopBits = ONESTOPBIT;
                dcbSerialParams.Parity = NOPARITY;
                if(!SetCommState(hSerial, &dcbSerialParams)) {
                    //error setting serial port state
                    ::CloseHandle(hSerial);
                }
            }

            // set timeouts
            COMMTIMEOUTS timeouts = {0};
            timeouts.ReadIntervalTimeout = 50;
            timeouts.ReadTotalTimeoutConstant = 50;
            timeouts.ReadTotalTimeoutMultiplier = 10;
            timeouts.WriteTotalTimeoutConstant = 50;
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