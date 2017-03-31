#pragma once

#include "cpplog/config.h"
#include "cpplog/utils.h"
#include "cpplog/attachment.h"
#include "cpplog/json_attachment.h"

namespace cpplog {

enum class LogLevel {
  trace,
  debug,
  info,
  warning,
  error,
  fatal
};

class Attachment;

class LogRecord {
public:
  LogRecord() = default;

  LogRecord(LogLevel level, const char* filename, const char* func, int line);

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

  LogLevel level_ = LogLevel::info;
  const char* file_name_ = "";
  const char* func_ = "";
  int line_ = 0;
  timespec timestamp_ = { 0 , 0 };
  JsonAttachment attachment_;
};

} // namespace cpplog

#include "cpplog/record_inl.h"

