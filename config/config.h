#pragma once
#include <string>
#include <SimpleIni.h> // https://github.com/brofield/simpleini

class config {
public:
    config(const std::string& name = "config.ini");

    std::string get_value(const std::string& key);
    void set_value(const std::string& key, const std::string& value);

    void commit();

private:
    bool is_dirty{ false };
    std::string ini_path;
    CSimpleIni ini;
};