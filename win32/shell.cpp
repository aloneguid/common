#include "shell.h"
#include <ShlObj_core.h>
#include <shellapi.h>
#include <filesystem>
#include "../../common/str.h"

using namespace std;
namespace fs = std::filesystem;

namespace win32 {
    namespace shell {
        static bool co_initialised{ false };

        static void ensure_co_initalised() {
            if (co_initialised) return;

            HRESULT ok = ::CoInitializeEx(0, COINIT_MULTITHREADED);

            co_initialised = true;
        }

        string get_shell_folder_path(int clsid) {
            TCHAR szPath[MAX_PATH];

            if(SUCCEEDED(SHGetFolderPath(nullptr,
                clsid,
                nullptr,
                0,
                szPath))) {
                return str::to_str(szPath);
            }

            return "";
        }

        string get_local_app_data_path() {
            return get_shell_folder_path(CSIDL_LOCAL_APPDATA);
        }

        std::string get_app_data_folder() {
            return get_shell_folder_path(CSIDL_APPDATA);
        }

        void exec(const std::string& path, const std::string& parameters) {
            HINSTANCE hi = ::ShellExecute(
                nullptr,
                L"open",
                str::to_wstr(path).c_str(),
                str::to_wstr(parameters).c_str(),
                nullptr,
                SW_SHOWDEFAULT);

            if (hi) {
                int i = 0;
            }

            /*fs::path fs_path(path);
            string work_dir = fs_path.parent_path();
            string process_name = fs_path.filename();

            STARTUPINFO si;
            PROCESS_INFORMATION pi;
            ::ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ::ZeroMemory(&pi, sizeof(pi));

            string all = string(L"\"") + path + L"\" " + parameters;

            if (!::CreateProcess(
                path.c_str(),
                (LPWSTR)parameters.c_str(),
                nullptr,
                nullptr,
                false,
                0,
                nullptr,
                work_dir.c_str(),
                &si,
                &pi))
            {
                DWORD err = ::GetLastError();
                if (err == ERROR_SUCCESS)
                {
                    return;
                }

            }

            ::CloseHandle(pi.hProcess);
            ::CloseHandle(pi.hThread);*/

        }

        void set_rounded_corners(HWND hWnd, int radius /*= 20*/) {
            RECT rect;
            if (::GetWindowRect(hWnd, &rect)) {
                HRGN rgn = CreateRoundRectRgn(0, 0, rect.right - rect.left, rect.bottom - rect.top, radius, radius);
                ::SetWindowRgn(hWnd, rgn, TRUE);
            }
        }

        void send_wm_copydata(HWND hWnd, const std::string& data, long data_type) {
            COPYDATASTRUCT cds;
            cds.dwData = data_type;
            cds.cbData = (data.size() + 1) * sizeof(wchar_t);
            cds.lpData = (LPVOID)(data.c_str());

            ::SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)(LPVOID)&cds);
        }

        std::string get_wm_copydata_data(WPARAM wParam, LPARAM lParam, long& data_type) {
            PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
            data_type = pcds->dwData; // data_type
            return str::to_str(static_cast<wchar_t*>(pcds->lpData));
        }

        void open_mssettings(const std::string name) {
            string url("ms-settings:");
            url += name;

            HINSTANCE hi = ::ShellExecute(
                nullptr,
                L"open",
                str::to_wstr(url).c_str(),
                nullptr,
                nullptr,
                SW_SHOWDEFAULT);
        }

        void open_default_apps() {
            open_mssettings("defaultapps");
        }

        shell_link read_link(const std::string& path) {
            // see https://renenyffenegger.ch/notes/Windows/development/WinAPI/Shell/read-lnk-file

            ensure_co_initalised();

            shell_link lnk;

            // create shell link interface
            IShellLink* shl;
            HRESULT rc = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&shl);
            if (SUCCEEDED(rc)) {

                // load file into link
                IPersistFile* ipf;
                rc = shl->QueryInterface(IID_IPersistFile, (LPVOID*)&ipf);
                if (SUCCEEDED(rc)) {
                    wstring wpath = str::to_wstr(path);
                    rc = ipf->Load(wpath.c_str(), STGM_READ);
                    if (SUCCEEDED(rc)) {
                        rc = shl->Resolve(0, 0);
                        if (SUCCEEDED(rc)) {
                            int ibuf;
                            const size_t buf_size = 1024;
                            wchar_t buf[buf_size];

                            rc = shl->GetPath(&buf[0], buf_size, 0, SLGP_RAWPATH);
                            if (SUCCEEDED(rc)) {
                                lnk.is_valid = true;
                                lnk.path = str::to_str(buf);
                            }

                            if (SUCCEEDED(shl->GetDescription(buf, buf_size))) {
                                lnk.description = str::to_str(buf);
                            }

                            if (SUCCEEDED(shl->GetArguments(buf, buf_size))) {
                                lnk.args = str::to_str(buf);
                            }

                            if (SUCCEEDED(shl->GetIconLocation(buf, buf_size, &ibuf))) {
                                lnk.icon = str::to_str(buf) + ":" + std::to_string(ibuf);
                            }

                            if (SUCCEEDED(shl->GetWorkingDirectory(&buf[0], buf_size))) {
                                lnk.pwd = str::to_str(buf);
                            }
                        }
                    }
                }
            }


            return lnk;
        }

        //BOOL GetDpiMonitorEnumProc(
        //    HMONITOR hMonitor,
        //    HDC hDC,
        //    LPRECT hRect,
        //    LPARAM lParam) {

        //    vector<HMONITOR>* hmons = (vector<HMONITOR>*)lParam;
        //    hmons->push_back(hMonitor);
        //    return true;
        //}

        unsigned int get_dpi() {
            //vector<HMONITOR> hmons;
            //::EnumDisplayMonitors(NULL, NULL, GetDpiMonitorEnumProc, (LPARAM)&hmons);

            //if (monitor_idx >= hmons.size()) return 0;

            return ::GetDpiForSystem();
        }
    }
}