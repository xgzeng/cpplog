#pragma once

#include "cpplog/config.h"
#include "json.hpp"  // nlohmann::json

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

class SourceFileInfo {
public:
  SourceFileInfo() {}

  SourceFileInfo(const char* file_name, const char* func, int line)
  : file_name_(file_name), func_(func), line_(line) {
  }

  const char* file_name() const {
    return file_name_;
  }

  const char* base_file_name() const {
    const char* base_filename = strrchr(file_name_, '/');
#ifdef WIN32
    if (!base_filename) base_filename = strrchr(file_name_, '\\');
#endif
    return base_filename ? base_filename + 1 : file_name_;
  }

  int line() const {
    return line_;
  }

  const char* function_name() const {
    return func_;
  }

private:
  const char* file_name_ = "";
  const char* func_ = "";
  int line_ = 0;
};

CPPLOG_INLINE void to_json(json& j, const SourceFileInfo& info) {
  j = json{{"file_name", info.file_name()},
           {"function_name", info.function_name()},
           {"line", info.line()}};
}

class LogRecord {
public:
  LogRecord() = default;

  LogRecord(LogLevel level, const SourceFileInfo&);

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

  const SourceFileInfo src_file_info() const {
    return src_file_info_;
  }

  void src_file_info(const SourceFileInfo& info) {
    src_file_info_ = info;
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

  SourceFileInfo src_file_info_;

  json fields_;
};

CPPLOG_INLINE LogRecord::LogRecord(LogLevel level,
                                   const SourceFileInfo& src_file_info)
: level_(level), timestamp_{0, 0}, src_file_info_(src_file_info) {
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

} // namespace cpplog

