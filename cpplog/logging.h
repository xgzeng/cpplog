#pragma once

#include <sstream>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <time.h>
#include "config.h"
#include "cpplog/json_builder.h"

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
  LogRecord() =  default;

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

class LogDispatcher : public LogSink {
public:
  static LogDispatcher& instance();

  bool IsEnabled(LogLevel level) const;

  void EnableLevelAbove(LogLevel level);

  void SubmitRecord(LogRecord& record) override;

  void AddLogSink(LogSink* sink);

  void RemoveLogSink(LogSink* sink);

  bool HasLogSink(LogSink* sink);

private:
  LogLevel level_limit_ = LogLevel::info;
  std::vector<LogSink*> sinks_;
};

class LogCapture {
public:
  LogCapture(LogLevel level, const char* filename,
             int line, const char* func);

  LogCapture(LogSink& sink, LogLevel level, const char* filename,
             int line, const char* func);
  ~LogCapture();

  template<typename T>
  LogCapture& operator << (T&& value) {
    message_stream_ << std::forward<T>(value);
    return *this;
  }

  template<typename... T>
  LogCapture& message(fmt::CStringRef fmt_str, T&&... args) {
    try {
      fmt::print(message_stream_, fmt_str, std::forward<T>(args)...);
    } catch (std::exception& e) {
      message_stream_ << "format message '" << fmt_str.c_str() << "' failed, " << e.what();
    }
    return *this;
  }

  // capture log record properties
  template<typename T>
  LogCapture& operator()(const std::string& name, T&& value) {
    JsonBuilder jb;
    dump(jb, std::forward<T>(value));
    record_.add_field(name, jb.ExtractString());
    return *this;
  }

private:
  LogRecord record_;
  LogSink& sink_;
  std::ostringstream message_stream_;
};

/// global functions
CPPLOG_INLINE void AddLogSink(LogSink*);

CPPLOG_INLINE void SetLogToConsole(bool enable);

} // namespace log

#include "logging-inl.h"

/// macros
constexpr cpplog::LogLevel LVL_DEBUG = cpplog::LogLevel::debug;
constexpr cpplog::LogLevel LVL_INFO = cpplog::LogLevel::info;
constexpr cpplog::LogLevel LVL_WARNING = cpplog::LogLevel::warning;
constexpr cpplog::LogLevel LVL_ERROR = cpplog::LogLevel::error;
constexpr cpplog::LogLevel LVL_FATAL = cpplog::LogLevel::fatal;

#ifdef __GNUC__
  #define __FUNCTION_INFO__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define __FUNCTION_INFO__ __FUNCSIG__
#else
  #define __FUNCTION_INFO__ __func__
#endif

#define LOG(level, fmt, ...) \
    if (cpplog::LogDispatcher::instance().IsEnabled(LVL_##level)) \
        cpplog::LogCapture(LVL_##level, __FILE__, __LINE__, __FUNCTION_INFO__)  \
            .message(fmt, ##__VA_ARGS__)

#define LOG_IF(level, condition, fmt, ...) \
    if (condition && cpplog::LogDispatcher::instance().IsEnabled(LVL_##level))  \
        cpplog::LogCapture(LVL_##level, __FILE__, __LINE__, __FUNCTION_INFO__)  \
            .message(fmt, ##__VA_ARGS__)

