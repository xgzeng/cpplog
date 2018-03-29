#pragma once

#include "cpplog/config.h"
#include "cpplog/attachment.h"

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
#if CPPLOG_DISABLE_ATTACHMENT
    attachment_.Attach(name, std::forward<T>(value));
#endif
  }

private:
  std::string message_;

  LogLevel level_ = LogLevel::Information;

  timespec timestamp_ = { 0 , 0 };

  source_location src_location_;

#if CPPLOG_DISABLE_ATTACHMENT
  Attachment attachment_;
#endif
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

} // namespace cpplog
