#pragma once
#include <string>
#include <map>
#include "../win32/http.h"

namespace azure
{
    class app_insights
    {
    public:
        app_insights(
            const std::string& instrumentation_key,
            const std::string& app_name,
            const std::string& app_version);

        void track_event(const std::string& name, std::map<std::string, std::string> props);

        void track_event(const std::string& name,
           const std::string& prop1_name = "", const std::string& prop1_value = "",
           const std::string& prop2_name = "", const std::string& prop2_value = "",
           const std::string& prop3_name = "", const std::string& prop3_value = "",
           const std::string& prop4_name = "", const std::string& prop4_value = "",
           const std::string& prop5_name = "", const std::string& prop5_value = "");

    private:
        std::string json_1;
        std::string json_2;
        std::string json_3;
        std::string json_4;

        bool is_async{ false };
        win32::http h;

        void http_post(const std::string& json_body);

    };
}

