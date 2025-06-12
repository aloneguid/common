#pragma once
#include <string>
#include <vector>
#include <SimpleIni.h> // https://github.com/brofield/simpleini

namespace common {
    class config {
    public:
        config(const std::string& path);

        std::string get_value(const std::string& key, const std::string& section = "") const;
        bool get_bool_value(const std::string& key, bool default_value = false, const std::string& section = "") const;
        int get_int_value(const std::string& key, int default_value = 0, const std::string& section = "") const;
        float get_float_value(const std::string& key, float default_value = 0.0f, const std::string& section = "") const;
        std::vector<std::string> get_all_values(const std::string& key, const std::string& section = "");

        void set_value(const std::string& key, const std::string& value, const std::string& section = "");
        void set_value(const std::string& key, const std::vector<std::string>& value, const std::string& section = "");
        void set_value(const std::string& key, bool value, const std::string& section = "");
        void set_value(const std::string& key, float value, const std::string& section = "");
        void set_value(const std::string& key, int value, const std::string& section = "");

        void delete_key(const std::string& key, const std::string& section = "");

        std::vector<std::string> list_sections();
        std::vector<std::string> list_keys(const std::string& section = "");
        void delete_section(const std::string& section);

        void reload();
        void commit();

        std::string get_absolute_path() { return ini_path; }

    private:
        bool is_dirty{ false };
        std::string ini_path;
        CSimpleIni ini;
    };
}