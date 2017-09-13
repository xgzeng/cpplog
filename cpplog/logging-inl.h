#pragma once

#include <algorithm>
#include <mutex>
#ifndef _WIN32
#include <sys/time.h>
#endif

#include "cpplog/sinks/console_sink.h"
#include "cpplog/sinks/file_sink.h"

namespace cpplog {

CPPLOG_INLINE bool LogDispatcher::is_level_enabled(LogLevel level) const {
  return level >= level_limit_;
}

CPPLOG_INLINE void LogDispatcher::EnableLevelAbove(LogLevel level) {
  level_limit_ = level;
}

CPPLOG_INLINE void LogDispatcher::Submit(const LogRecord& r) {
  for (auto s : sinks_) {
    s->Submit(r);
  }
}

CPPLOG_INLINE void LogDispatcher::AddLogSink(LogSink* sink) {
  LogSinkPtr p_sink {sink, [](LogSink*){}};
  AddLogSink(p_sink);
}

CPPLOG_INLINE void LogDispatcher::AddLogSink(LogSinkPtr sink) {
  sinks_.push_back(sink);
}

CPPLOG_INLINE void LogDispatcher::RemoveLogSink(LogSink* sink) {
  auto p = std::remove_if(sinks_.begin(), sinks_.end(),
      [sink](const LogSinkPtr& p) { return p.get() == sink; });

  sinks_.erase(p, sinks_.end());
}

CPPLOG_INLINE bool LogDispatcher::HasLogSink(LogSink* sink) {
  auto found = std::find_if(sinks_.begin(), sinks_.end(),
      [sink](const LogSinkPtr& p) { return p.get() == sink; });

  return  found != sinks_.end();
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

namespace detail {
  
CPPLOG_INLINE void SetLogToFileHelper(FileSink& sink) {
}

template<typename T1, typename... Ts>
CPPLOG_INLINE void SetLogToFileHelper(FileSink& sink, T1&& modifier1, Ts&&... modifiers) {
  modifier1(sink);
  SetLogToFileHelper(sink, modifiers...);
}

} // namespace detail

template<typename... T>
CPPLOG_INLINE void SetLogToFile(T&&... modifiers) {
  auto psink = std::make_shared<FileSink>();
  
  detail::SetLogToFileHelper(*psink, modifiers...);
  
  LogDispatcher::instance().AddLogSink(psink);
}

// LogCapturer
CPPLOG_INLINE LogCapture::LogCapture(LogLevel level, const source_location& src_location)
: record_(level, src_location), sink_(LogDispatcher::instance()) {
}

CPPLOG_INLINE LogCapture::LogCapture(LogSink& s,
                                     LogLevel level,
                                     const source_location& src_location)
: record_(level, src_location), sink_(s) {
}

CPPLOG_INLINE LogCapture::~LogCapture() {
  record_.message(message_stream_.str());
  sink_.Submit(record_);
}

} // namespace cpplog
