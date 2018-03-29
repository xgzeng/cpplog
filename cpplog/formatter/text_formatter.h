#pragma once
#include "cpplog/config.h"
#include "cpplog/record.h"

#include <fmt/format.h>

namespace cpplog {

namespace {
  
const char* base_file_name(const char* file_name) {
  const char* base_filename = strrchr(file_name, '/');
#ifdef _WIN32
  if (!base_filename) base_filename = strrchr(file_name, '\\');
#endif
  return base_filename ? base_filename + 1 : file_name;
}

}

CPPLOG_INLINE std::string FormatAsText(const LogRecord& r) {
  char level_letter = [&r]() {
    switch (r.level()) {
    case LogLevel::Trace:       return 'T';
    case LogLevel::Debug:       return 'D';
    case LogLevel::Information: return 'I';
    case LogLevel::Warning:     return 'W';
    case LogLevel::Error:       return 'E';
    case LogLevel::Fatal:       return 'F';
    default: return 'I';
    }
  }();

  struct tm tm_local {};
#ifdef _WIN32
  localtime_s(&tm_local, &r.timestamp().tv_sec);
#else
  localtime_r(&r.timestamp().tv_sec, &tm_local);
#endif

  auto& file_info = r.src_location();

  return fmt::format("{}{:0>2}{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0>4} {}:{}:{}] {}",
      level_letter, 1 + tm_local.tm_mon, tm_local.tm_mday,
      tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec,
      r.timestamp().tv_nsec / 1000000,
      base_file_name(file_info.file_name()),  file_info.line(), file_info.function_name(),
      r.message()
  );
}

} // namespace cpplog