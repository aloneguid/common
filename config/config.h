#pragma once
#include <string>
#include <vector>
#include <SimpleIni.h> // https://github.com/brofield/simpleini

namespace common {
    class config {
    public:
        config(const std::string& name = "config.ini");

        std::string get_value(const std::string& key, const std::string& section = "");
        std::vector<std::string> get_all_values(const std::string& key, const std::string& section = "");

        void set_value(const std::string& key, const std::string& value, const std::string& section = "");
        void set_value(const std::string& key, const std::vector<std::string>& value, const std::string& section = "");

        void delete_key(const std::string& key, const std::string& section = "");

        std::vector<std::string> list_sections();
        void delete_section(const std::string& section);

        void commit();

    private:
        bool is_dirty{ false };
        std::string ini_path;
        CSimpleIni ini;
    };
}