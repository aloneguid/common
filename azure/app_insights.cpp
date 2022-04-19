#include <iostream>
#include "app_insights.h"
#include "../win32/http.h"
#include <fmt/core.h>
#include "../time.h"

using namespace std;

namespace azure
{
   void app_insights::http_post(const string& json_body)
   {
      h.post("dc.services.visualstudio.com", "/v2/track", json_body);
   }


   app_insights::app_insights(const std::string& instrumentation_key, const std::string& app_name, const std::string& app_version)
   {
      //{"name":"AppEvents","time":"2021-07-21T10:58:09.9662971Z","iKey":"f98a0d37-2e22-4b25-9a82-a559a2c7ee4b","tags":{"ai.application.ver":"1.0.0.0","ai.cloud.roleInstance":"sb","ai.internal.sdkVersion":"dotnetc:2.17.0-146","ai.internal.nodeName":"sb"},"data":{"baseType":"EventData","baseData":{"ver":2,"name":"appOpen","properties":{"name":"bt","version":"1.0"}}}}
      json_1 = R"({"name":"AppEvents","iKey":")";
      json_1 += instrumentation_key;
      json_1 += R"(", "time": ")";
      // <--- time
      json_2 = R"(Z","data":{"baseType":"EventData","baseData":{"ver":2,"name":")";
      // <--- event name
      json_3 = R"(","properties":{"name":")";
      json_3 += app_name;
      json_3 += R"(","version":")";
      json_3 += app_version;
      json_3 += "\"";
      // <--- any other extra properties
      json_4 = R"(}}}})";
   }

   void app_insights::track_event(const std::string& name,
      const std::string& prop1_name, const std::string& prop1_value,
      const std::string& prop2_name, const std::string& prop2_value,
      const std::string& prop3_name, const std::string& prop3_value,
      const std::string& prop4_name, const std::string& prop4_value)
   {
      map<string, string> params;
      if (!prop1_name.empty()) params[prop1_name] = prop1_value;
      if (!prop2_name.empty()) params[prop2_name] = prop2_value;
      if (!prop3_name.empty()) params[prop3_name] = prop3_value;
      if (!prop4_name.empty()) params[prop4_name] = prop4_value;
      track_event(name, params);
   }

   void app_insights::track_event(
      const std::string& name,
      std::map<std::string, std::string> props)
   {
      string body(json_1);
      body += times::to_iso_8601();
      body += json_2;
      body += name;
      body += json_3;

      for (const auto& pair : props)
      {
         if (!pair.first.empty())
         {
            body += fmt::format(", \"{}\": \"{}\"", pair.first, pair.second);
         }
      }
      body += json_4;

      http_post(body);
   }

}