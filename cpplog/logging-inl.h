#pragma once
#include <cpplog/config.h>
#include <mutex>
#include "console_sink.h"

namespace cpplog {

CPPLOG_INLINE LogRecord::LogRecord(LogLevel level,
                            const char* filename,
                            const char* func,
                            int line)
: level_(level), file_name_(filename), func_(func), line_(line),
  timestamp_{0, 0} {

  struct timeval tv;
  if (0 == gettimeofday(&tv, nullptr)) {
    timestamp_.tv_sec = tv.tv_sec;
    timestamp_.tv_nsec = tv.tv_usec * 1000;
  }
//  clock_gettime(CLOCK_REALTIME, &timestamp_);
}

CPPLOG_INLINE const std::string& LogRecord::field(string_view name) {
  for(auto& field : fields_) {
    if (field.first == name) {
      return field.second;
    }
  }
  throw std::runtime_error("no such field");
}

CPPLOG_INLINE void LogDispatcher::SubmitRecord(LogRecord& r) {
  for (auto s : sinks_) {
    s->SubmitRecord(r);
  }
}

CPPLOG_INLINE void LogDispatcher::AddLogSink(LogSink* sink) {
  sinks_.push_back(sink);
}

CPPLOG_INLINE void LogDispatcher::RemoveLogSink(LogSink* sink) {
  sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), sink),
               sinks_.end());
}

CPPLOG_INLINE bool LogDispatcher::HasLogSink(LogSink* sink) {
  return std::find(sinks_.begin(), sinks_.end(), sink) != sinks_.end();
}

CPPLOG_INLINE ConsoleSink* console_sink() {
  static ConsoleSink console_sink;
  return &console_sink;
}

CPPLOG_INLINE LogDispatcher& LogDispatcher::instance() {
  static LogDispatcher global_instance;
  static std::once_flag once;
  std::call_once(once, [&](){
    global_instance.AddLogSink(console_sink());
  });

  return global_instance;
};

CPPLOG_INLINE void AddLogSink(LogSink* sink) {
  if (!LogDispatcher::instance().HasLogSink(sink)) {
      LogDispatcher::instance().AddLogSink(sink);
  }
}

CPPLOG_INLINE void SetLogToConsole(bool enable) {
  if (enable) {
    if (!LogDispatcher::instance().HasLogSink(console_sink())) {
        LogDispatcher::instance().AddLogSink(console_sink());
    }
  } else {
    LogDispatcher::instance().RemoveLogSink(console_sink());
  }
}

// LogCapturer
CPPLOG_INLINE LogCapture::LogCapture(LogLevel level, const char* filename,
                                     int line, const char* func)
: sink_(LogDispatcher::instance()), record_(level, filename, func, line) {
}

CPPLOG_INLINE LogCapture::LogCapture(LogSink& s,
                                     LogLevel level,
                                     const char* filename,
                                     int line,
                                     const char* func)
: sink_(s), record_(level, filename, func, line) {
}

CPPLOG_INLINE LogCapture::~LogCapture() {
  record_.message(message_stream_.str());
  sink_.SubmitRecord(record_);
}

}

