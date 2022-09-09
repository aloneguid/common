#include "config.h"
#include "../fss.h"

config::config(const std::string& name) {
    ini_path = fss::get_current_dir() + "/" + name + ".ini";
    ini.LoadFile(ini_path.c_str());
}

std::string config::get_value(const std::string& key) {
    const char* v = ini.GetValue("", key.c_str());
    return v ? v : "";
}

void config::set_value(const std::string& key, const std::string& value) {
    ini.SetValue("", key.c_str(), value.c_str());
    is_dirty = true;
}

void config::commit() {
    if (!is_dirty) return;

    ini.SaveFile(ini_path.c_str());
    is_dirty = false;
}

using namespace std;