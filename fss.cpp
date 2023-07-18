#include "fss.h"
#if WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "str.h"
#include <fstream>
#include <vector>

using namespace std;

namespace fss
{
    std::string get_current_dir() {

#if WIN32
        TCHAR buffer[MAX_PATH] = {0};
        ::GetModuleFileName(nullptr, buffer, MAX_PATH);
        std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
        auto r = std::wstring(buffer).substr(0, pos);
        return str::to_str(r);
#else
        //long size = pathconf(".", _PC_PATH_MAX);
        //getcwd()
        return "";
#endif
    }

    std::string get_current_exec_path() {
#if WIN32
        TCHAR szFileName[MAX_PATH];
        ::GetModuleFileName(nullptr, szFileName, MAX_PATH);
        return str::to_str(szFileName);
#else
        return "";
#endif
    }

    bool file_exists(const std::string& name) {
        std::ifstream f{name};
        return f.is_open();
    }

    size_t get_file_size(const std::string& name) {
        std::ifstream fl(name, std::ifstream::ate | std::ifstream::binary);
        if(!fl) return -1;
        return fl.tellg();
    }

    bool read_binary_file(const std::string& name, unsigned char* buffer) {
        std::ifstream fl(name, std::ifstream::ate | std::ifstream::binary);
        if(!fl) return false;
        auto size = fl.tellg();
        fl.seekg(fl.beg);

        fl.read((char*)buffer, size);
        return true;
    }

    std::string read_file_as_string(const std::string& name) {
        ifstream ifs(name, ios::in | ios::binary | ios::ate);
        if(!ifs) return "";
        ifstream::pos_type sz = ifs.tellg();
        ifs.seekg(0, ios::beg);
        vector<char> bytes(sz);
        ifs.read(bytes.data(), sz);
        return string(bytes.data(), sz);
    }

    void write_file_as_string(const std::string& filename, const std::string& content) {
        ofstream ofs(filename, ios::out | ios::binary | ios::ate);
        if(!ofs) return;

        ofs << content;
        ofs.flush();
    }
}