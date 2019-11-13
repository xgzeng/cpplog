#pragma once

#include <cpplog/logging.h>
#include <glog/logging.h>

namespace cpplog {

class GLogSink : public LogSink {
public:
  void Submit(const LogRecord& r) override {
    google::LogSeverity severity = google::GLOG_INFO;
    switch (r.level()) {
    case LogLevel::Trace:
    case LogLevel::Debug:
    case LogLevel::Information:    severity = google::GLOG_INFO; break;
    case LogLevel::Warning: severity = google::GLOG_WARNING; break;
    case LogLevel::Error:   severity = google::GLOG_ERROR; break;
    case LogLevel::Fatal:   severity = google::GLOG_FATAL; break;
    default:
      break;
    }
    auto& src_location = r.src_location();
    google::LogMessage(src_location.file_name(), src_location.line(), severity, 0,
                       &google::LogMessage::SendToLog).stream() << r.message();
  }
};

}
