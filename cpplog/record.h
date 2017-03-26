#pragma once

#include "cpplog/config.h"

namespace cpplog {

enum class LogLevel {
  trace,
  debug,
  info,
  warning,
  error,
  fatal
};

class Attachment {
public:
  bool empty() const {
    return map_size == 0;
  }

  void add(const std::string& name, const std::string value);

  void add(const std::string& name, int value);

private:
  int map_size = 0;
  std::string msg_pack_map_data;
};

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

  Attachment& attachment() {
    return attachment_;
  }

  const Attachment& attachment() const {
    return attachment_;
  }

private:
  std::string message_;

  LogLevel level_ = LogLevel::info;
  const char* file_name_ = "";
  const char* func_ = "";
  int line_ = 0;
  timespec timestamp_ = { 0 , 0 };
  Attachment attachment_;
};

} // namespace cpplog
