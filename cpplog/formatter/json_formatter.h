#pragma once
#include "cpplog/config.h"
#include "cpplog/record.h"
#include "cpplog/nlohmann_json.h"

#include <fmt/time.h>

namespace cpplog {

CPPLOG_INLINE std::string to_iso8601(timespec ts) {
  struct tm result;
#ifdef _WIN32
  gmtime_s(&result, &ts.tv_sec);
#else
  gmtime_r(&ts.tv_sec, &result);
#endif
  return fmt::format("{:%FT%T}.{:03}Z", result, ts.tv_nsec/1000/1000); //  / (1000 * 1000)
}

CPPLOG_INLINE std::string FormatAsJSON(const LogRecord& record) {
  json j{
    {"@timestamp", to_iso8601(record.timestamp())},
    {"@version", "1"},
    {"level", record.level()},
    {"message", record.message()},
    {"source_location", record.src_location()}
  };
//   json j{};
//   j["@timestamp"] = to_iso8601(record.timestamp());
//   j["@version"] = "1";
//   j["level"] = record.level();
//   j["message"] = record.message();
//   j["source_location"] = record.src_location();
  return j.dump();
}

} // namespace cpplog