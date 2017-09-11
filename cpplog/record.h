#pragma once

#include "cpplog/config.h"
#include "json.hpp"      // nlohmann::json
#include "fmt/format.h"

#ifndef WIN32
#include <sys/time.h> // gettimeofday
#endif

namespace cpplog {

using json = nlohmann::json;

enum class LogLevel {
  Trace,
  Debug,
  Information,
  Warning,
  Error,
  Fatal
};

CPPLOG_INLINE void to_json(json& j, LogLevel l) {
  switch(l) {
  case LogLevel::Trace:          j = "TRACE"; break;
  case LogLevel::Debug:          j = "DEBUG"; break;
  case LogLevel::Information:    j = "INFO";  break;
  case LogLevel::Warning:        j = "WARNING"; break;
  case LogLevel::Error:          j = "ERROR"; break;
  case LogLevel::Fatal:          j = "FATAL"; break;
  }
}

class source_location {
public:
  source_location() noexcept {}
  
  source_location(const char* file_name, const char* func, int line)
  : file_name_(file_name), function_name_(func), line_(line) {
  }

  constexpr const char* file_name() const noexcept {
    return file_name_;
  }

  const char* base_file_name() const {
    const char* base_filename = strrchr(file_name_, '/');
#ifdef WIN32
    if (!base_filename) base_filename = strrchr(file_name_, '\\');
#endif
    return base_filename ? base_filename + 1 : file_name_;
  }

  constexpr const char* function_name() const {
    return function_name_;
  }

  constexpr std::uint_least32_t column() const noexcept {
    return column_;
  }
  
  constexpr std::uint_least32_t line() const noexcept {
    return line_;
  }

private:
  const char* file_name_ {""};
  const char* function_name_ {""};
  std::uint_least32_t line_ {0};
  std::uint_least32_t column_ {0};
};

CPPLOG_INLINE void to_json(json& j, const source_location& info) {
  j = json{{"file_name", info.file_name()},
           {"function_name", info.function_name()},
           {"line", info.line()}};
}

class LogRecord {
public:
  LogRecord() = default;

  LogRecord(LogLevel level, const source_location&);

  ~LogRecord() = default;

  LogRecord& operator=(const LogRecord&) = default;

  std::string& message() {
    return message_;
  };

  const std::string& message() const {
    return message_;
  };

  void message(std::string msg) {
    message_ = std::move(msg);
  }

  LogLevel level() const {
    return level_;
  }

  const source_location src_location() const {
    return src_location_;
  }

  void src_location(const source_location& info) {
    src_location_ = info;
  }

  const timespec& timestamp() const {
    return timestamp_;
  }

  // add integer property
  template<typename T>
  void add_field(string_view name, T&& value) {
    fields_[std::string(name)] = value;
  }

  json fields() const {
    return fields_;
  };

private:
  std::string message_;

  LogLevel level_ = LogLevel::Information;

  timespec timestamp_ = { 0 , 0 };

  source_location src_location_;

  json fields_;
};

CPPLOG_INLINE LogRecord::LogRecord(LogLevel level,
                                   const source_location& src_loc)
: level_(level), timestamp_{0, 0}, src_location_(src_loc) {
#ifdef _WIN32
  timespec_get(&timestamp_, TIME_UTC);
#else
  struct timeval tv;
  if (0 == gettimeofday(&tv, nullptr)) {
    timestamp_.tv_sec = tv.tv_sec;
    timestamp_.tv_nsec = tv.tv_usec * 1000;
  }
#endif
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
#ifdef WIN32
  localtime_s(&tm_local, &r.timestamp().tv_sec);
#else
  localtime_r(&r.timestamp().tv_sec, &tm_local);
#endif

  auto& file_info = r.src_location();

  return fmt::format("{}{:0>2}{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0>4} {}:{}:{}] {}",
      level_letter, 1 + tm_local.tm_mon, tm_local.tm_mday,
      tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec,
      r.timestamp().tv_nsec / 1000000,
      file_info.base_file_name(),  file_info.line(), file_info.function_name(),
      r.message()
  );
}

} // namespace cpplog
