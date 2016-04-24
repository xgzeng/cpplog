#pragma once

#include "cpplog/logging.h"
#include <glog/logging.h>

namespace cpplog {

class GLogSink : public LogSink {
public:
  void SubmitRecord(LogRecord& r) override {
    google::LogSeverity severity = google::GLOG_INFO;
    switch (r.level()) {
    case LogLevel::trace:
    case LogLevel::debug:
    case LogLevel::info:    severity = google::GLOG_INFO; break;
    case LogLevel::warning: severity = google::GLOG_WARNING; break;
    case LogLevel::error:   severity = google::GLOG_ERROR; break;
    case LogLevel::fatal:   severity = google::GLOG_FATAL; break;
    default:
      break;
    }
    google::LogMessage(r.file_name(), r.line(), severity, 0, &google::LogMessage::SendToLog).stream() << r.message();
  }
};

}

