#pragma once
#include <string>
#include <chrono>

namespace times
{
   std::string to_iso_8601(std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now());
}