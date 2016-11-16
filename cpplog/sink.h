#pragma once

#include <cpplog/config.h>
#include <string>
#include <time.h>
#include <vector>

namespace cpplog {

enum class LogLevel {
  trace,
  debug,
  info,
  warning,
  error,
  fatal
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

  typedef std::pair<std::string, std::string> name_json_pair;

  void add_field(const std::string& name, std::string value) {
	fields_.emplace_back(name, std::move(value));
  }

  const std::string& field(const std::string& name);

  std::vector<name_json_pair>& fields() {
	return fields_;
  }

  const std::vector<name_json_pair>& fields() const {
	return fields_;
  }

private:
  std::string message_;

  LogLevel level_ = LogLevel::info;
  const char* file_name_ = "";
  const char* func_ = "";
  int line_ = 0;
  timespec timestamp_ = { 0 , 0 };

  std::vector<name_json_pair> fields_;
};

class LogSink {
public:
  virtual void SubmitRecord(LogRecord&) = 0;
protected:
  ~LogSink() = default;
};

}  // namespace cpplog
