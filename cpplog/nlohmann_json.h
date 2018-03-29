#pragma once

#include "cpplog/config.h"
#include "cpplog/record.h"

#include <nlohmann/json.hpp>      // nlohmann::json

namespace cpplog {

typedef nlohmann::json json;

CPPLOG_INLINE void to_json(nlohmann::json& j, LogLevel l) {
  switch(l) {
  case LogLevel::Trace:          j = "TRACE"; break;
  case LogLevel::Debug:          j = "DEBUG"; break;
  case LogLevel::Information:    j = "INFO";  break;
  case LogLevel::Warning:        j = "WARNING"; break;
  case LogLevel::Error:          j = "ERROR"; break;
  case LogLevel::Fatal:          j = "FATAL"; break;
  }
}

CPPLOG_INLINE void to_json(json& j, const source_location& info) {
  j = json{{"file_name", info.file_name()},
           {"function_name", info.function_name()},
           {"line", info.line()}};
}

} // namespace cpplog