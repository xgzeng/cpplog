#pragma once

#include "cpplog/config.h"
#include "cpplog/attachment.h"

#include <fmt/format.h>

#ifndef _WIN32
#include <sys/time.h> // gettimeofday
#else
#include <time.h>
#endif

namespace cpplog {

enum class LogLevel {
  Trace,
  Debug,
  Information,
  Warning,
  Error,
  Fatal
};

class LogRecord {
public:
  LogRecord() = default;
  
  LogRecord(string_view msg);

  LogRecord(LogLevel level, const source_location&);

  ~LogRecord() = default;

  LogRecord& operator=(const LogRecord&) = default;

  const std::string& message() const {
    return message_;
  };

  void set_message(string_view msg) {
#ifdef __cpp_lib_experimental_string_view
    message_ = std::string(msg.data(), msg.size());
#else
    message_ = msg;
#endif
  }

  LogLevel level() const {
    return level_;
  }

  void set_leve(LogLevel value) {
    level_ = value;
  }

  const source_location src_location() const {
    return src_location_;
  }

  void set_src_location(const source_location& value) {
    src_location_ = value;
  }

  const timespec& timestamp() const {
    return timestamp_;
  }

  void set_timestamp(const timespec& value) {
    timestamp_ = value;
  }

  template<typename T>
  void Attach(string_view name, T&& value) {
    attachment_.Attach(name, std::forward<T>(value));
  }

private:
  std::string message_;

  LogLevel level_ = LogLevel::Information;

  timespec timestamp_ = { 0 , 0 };

  source_location src_location_;

  Attachment attachment_;
};

CPPLOG_INLINE LogRecord::LogRecord(string_view msg) {
#ifdef __cpp_lib_experimental_string_view
    message_ = std::string(msg.data(), msg.size());
#else
    message_ = msg;
#endif
}

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
