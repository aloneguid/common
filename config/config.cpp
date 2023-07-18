#include "config.h"
#include "../fss.h"
#include <list>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

namespace common {
    config::config(const std::string& path) {
        ini_path = path;

        // create dir if not exists
        fs::path abs{path};
        fs::create_directories(abs.parent_path());

        ini.SetMultiKey(true);
        ini.LoadFile(ini_path.c_str());
    }

    std::string config::get_value(const std::string& key, const std::string& section) const {
        const char* v = ini.GetValue(section.c_str(), key.c_str());
        return v ? v : "";
    }

    bool config::get_bool_value(const std::string& key, bool default_value, const std::string& section) const {
        string v = get_value(key, section);
        if(!v.empty()) {
            if(v == "y" || v == "1") return true;
            if(v == "n" || v == "0") return false;
        }
        return default_value;
    }

    std::vector<std::string> config::get_all_values(const std::string& key, const std::string& section) {
        vector<string> r;
        list<CSimpleIni::Entry> ir;
        ini.GetAllValues(section.c_str(), key.c_str(), ir);
        for (auto& e : ir) {
            r.push_back(e.pItem);
        }
        return r;
    }

    void config::set_value(const std::string& key, const std::string& value, const std::string& section) {
        ini.Delete(section.c_str(), key.c_str());
        if(!value.empty())
            ini.SetValue(section.c_str(), key.c_str(), value.c_str());
        is_dirty = true;
    }

    void config::set_value(const std::string& key, const std::vector<std::string>& value, const std::string& section) {

        ini.Delete(section.c_str(), key.c_str());
        for (const string& one_value : value) {
            ini.SetValue(section.c_str(), key.c_str(), one_value.c_str());
        }
        is_dirty = true;
    }

    void config::set_bool_value(const std::string& key, bool value, const std::string& section) {
        set_value(key, value ? "y" : "n", section);
    }

    void config::delete_key(const std::string& key, const std::string& section) {
        ini.Delete(section.c_str(), key.c_str(), true);
        is_dirty = true;
    }

    std::vector<std::string> config::list_sections() {
        vector<string> r;
        list<CSimpleIni::Entry> ir;
        ini.GetAllSections(ir);
        for (auto& s : ir) {
            r.push_back(s.pItem);
        }
        return r;
    }

    void config::delete_section(const std::string& section) {
        ini.Delete(section.c_str(), nullptr, true);
        is_dirty = true;
    }

    void config::commit() {
        if (!is_dirty) return;

        ini.SaveFile(ini_path.c_str());
        is_dirty = false;
    }
}