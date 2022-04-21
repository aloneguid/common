#include "time.h"
#include <iomanip>

namespace times
{
   std::string to_iso_8601(std::chrono::time_point<std::chrono::system_clock> t)
   {
      // convert to time_t which will represent the number of
      // seconds since the UNIX epoch, UTC 00:00:00 Thursday, 1st. January 1970
      time_t epoch_seconds = std::chrono::system_clock::to_time_t(t);

      // Format this as date time to seconds resolution
      // e.g. 2016-08-30T08:18:51
      std::stringstream stream;
      struct tm buf;
      gmtime_s(&buf, &epoch_seconds);
      stream << std::put_time(&buf, "%FT%T");

      // If we now convert back to a time_point we will get the time truncated
      // to whole seconds 
      auto truncated = std::chrono::system_clock::from_time_t(epoch_seconds);

      // Now we subtract this seconds count from the original time to
      // get the number of extra microseconds..
      auto delta_us = std::chrono::duration_cast<std::chrono::microseconds>(t - truncated).count();

      // And append this to the output stream as fractional seconds
      // e.g. 2016-08-30T08:18:51.867479
      stream << "." << std::fixed << std::setw(6) << std::setfill('0') << delta_us;

      return stream.str();
   }
}