#pragma once

#include "cpplog/config.h"
#include "cpplog/sink.h"
#include "cpplog/record.h"
#include <time.h>
#include <sstream>
#include "fmt/format.h"
#include "fmt/ostream.h"

namespace cpplog {

class LogDispatcher : public LogSink {
public:
  static LogDispatcher& instance();

  bool is_level_enabled(LogLevel level) const override;

  void EnableLevelAbove(LogLevel level);

  void SubmitRecord(const LogRecord& record) override;

  void AddLogSink(LogSink* sink);

  void RemoveLogSink(LogSink* sink);

  bool HasLogSink(LogSink* sink);

  template<typename T, typename... ConstructorArgs>
  void AddSink(ConstructorArgs... args) {
    auto p = std::make_shared<T>(std::forward<ConstructorArgs>(args)...);
    sinks_.push_back(p);
  }

private:
  typedef std::shared_ptr<LogSink> LogSinkPtr;
  LogLevel level_limit_ = LogLevel::Information;
  std::vector<LogSinkPtr> sinks_;
};

class LogCapture {
public:
  LogCapture(LogLevel level, const SourceFileInfo&);

  LogCapture(LogSink& sink, LogLevel level, const SourceFileInfo&);

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
  LogCapture& operator()(string_view name, T&& value) {
    record_.add_field(name, std::forward<T>(value));
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

#include "cpplog/logging-inl.h"

/// macros
constexpr cpplog::LogLevel LVL_TRACE   = cpplog::LogLevel::Trace;
constexpr cpplog::LogLevel LVL_DEBUG   = cpplog::LogLevel::Debug;
constexpr cpplog::LogLevel LVL_INFO    = cpplog::LogLevel::Information;
constexpr cpplog::LogLevel LVL_WARNING = cpplog::LogLevel::Warning;
constexpr cpplog::LogLevel LVL_ERROR   = cpplog::LogLevel::Error;
constexpr cpplog::LogLevel LVL_FATAL   = cpplog::LogLevel::Fatal;

#ifdef __GNUC__
  #define __FUNCTION_SIGNATURE__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define __FUNCTION_SIGNATURE__ __FUNCSIG__
#else
  #define __FUNCTION_SIGNATURE__ ""
#endif

#define LOG_TO_IMPL(sink, level, fmt, ...) \
  if (sink.is_level_enabled(level))   \
    cpplog::LogCapture(sink, level, {__FILE__, __func__, __LINE__})  \
            .message(fmt, ##__VA_ARGS__)

#define LOG_TO(sink, level, fmt, ...) \
    LOG_TO_IMPL(sink, LVL_##level, fmt, ##__VA_ARGS__)

#define LOG(level, fmt, ...) \
  LOG_TO_IMPL(cpplog::LogDispatcher::instance(), LVL_##level, fmt, ##__VA_ARGS__)

#define LOG_TO_IF(sink, level, condition,  fmt, ...) \
  if (condition) LOG_TO_IMPL(sink, LVL_##level, fmt, ##__VA_ARGS__)

#define LOG_IF(level, condition, fmt, ...) \
  if (condition) LOG_TO_IMPL(cpplog::LogDispatcher::instance(), \
    LVL_##level, fmt, ##__VA_ARGS__)
