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
  LogCapture& operator()(string_view name, T&& value) {
    //JsonBuilder jb;
    // dump(jb, std::forward<T>(value));
    // jb.ExtractString()
    record_.attachment().add(name, std::forward<T>(value));
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
constexpr cpplog::LogLevel LVL_DEBUG = cpplog::LogLevel::debug;
constexpr cpplog::LogLevel LVL_INFO = cpplog::LogLevel::info;
constexpr cpplog::LogLevel LVL_WARNING = cpplog::LogLevel::warning;
constexpr cpplog::LogLevel LVL_ERROR = cpplog::LogLevel::error;
constexpr cpplog::LogLevel LVL_FATAL = cpplog::LogLevel::fatal;

#ifdef __GNUC__
  #define __FUNCTION_SIGNATURE__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define __FUNCTION_SIGNATURE__ __FUNCSIG__
#else
  #define __FUNCTION_SIGNATURE__ ""
#endif

#define LOG(level, fmt, ...) \
    if (cpplog::LogDispatcher::instance().IsEnabled(LVL_##level)) \
        cpplog::LogCapture(LVL_##level, __FILE__, __LINE__, __func__)  \
            .message(fmt, ##__VA_ARGS__)

#define LOG_IF(level, condition, fmt, ...) \
    if (condition && cpplog::LogDispatcher::instance().IsEnabled(LVL_##level))  \
        cpplog::LogCapture(LVL_##level, __FILE__, __LINE__, __func__)  \
            .message(fmt, ##__VA_ARGS__)

