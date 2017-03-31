#pragma once

#include "cpplog/config.h"
#include "cpplog/attachment.h"

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

  LogRecord(LogLevel level,
            const char* file_name,
            const char* function_name,
            int line);

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

  const char* file_name() const {
    return file_name_;
  }

  int line() const {
    return line_;
  }

  const char* function_name() const {
    return func_;
  }

  const timespec& timestamp() const {
    return timestamp_;
  }

  JsonAttachment& attachment() {
    return attachment_;
  }

  const JsonAttachment& attachment() const {
    return attachment_;
  }

private:
  std::string message_;

  LogLevel level_ = LogLevel::Information;
  const char* file_name_ = "";
  const char* func_ = "";
  int line_ = 0;
  timespec timestamp_ = { 0 , 0 };

  JsonAttachment attachment_;
};

CPPLOG_INLINE LogRecord::LogRecord(LogLevel level,
                                   const char* filename,
                                   const char* func,
                                   int line)
: level_(level), file_name_(filename), func_(func),
  line_(line), timestamp_{0, 0} {
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

