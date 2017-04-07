#pragma once

#include "cpplog/config.h"
#include "cpplog/record.h"

namespace cpplog {

class LogSink {
public:
  virtual bool is_level_enabled(LogLevel level) const {
    return true;
  }

  virtual void SubmitRecord(const LogRecord&) = 0;

protected:
  ~LogSink() = default;
};

}  // namespace cpplog

