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

class CompositeSink : public LogSink {
public:
  CompositeSink() {}

  void SubmitRecord(LogRecord& r) override {
    for (auto s : sinks_) {
      s->SubmitRecord(r);
    }
  }

  void AddLogSink(LogSink* sink) {
    sinks_.push_back(sink);
  }

  void RemoveLogSink(LogSink* sink) {
    sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), sink),
                 sinks_.end());
  }

  bool HasLogSink(LogSink* sink) {
    return std::find(sinks_.begin(), sinks_.end(), sink) != sinks_.end();
  }

private:
  std::vector<LogSink*> sinks_;
};

CPPLOG_INLINE ConsoleSink* console_sink() {
  static ConsoleSink console_sink;
  return &console_sink;
}

CPPLOG_INLINE CompositeSink* GlobalSink() {
  static CompositeSink composite_sink;
  static std::once_flag once;
  std::call_once(once, [&](){
    composite_sink.AddLogSink(console_sink());
  });

  return &composite_sink;
};

CPPLOG_INLINE void AddLogSink(LogSink* sink) {
  if (!GlobalSink()->HasLogSink(sink)) {
    GlobalSink()->AddLogSink(sink);
  }
}

CPPLOG_INLINE void SetLogToConsole(bool enable) {
  auto gs = GlobalSink();
  if (enable) {
    if (!gs->HasLogSink(console_sink())) {
      gs->AddLogSink(console_sink());
    }
  } else {
    gs->RemoveLogSink(console_sink());
  }
}

// LogCapturer
CPPLOG_INLINE LogCapture::LogCapture(LogLevel level, const char* filename,
                                     int line, const char* func)
: sink_(*GlobalSink()), record_(level, filename, func, line) {
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

