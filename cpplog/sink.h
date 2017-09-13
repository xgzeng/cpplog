#pragma once

#include "cpplog/config.h"
#include "cpplog/record.h"

namespace cpplog {

class LogSink {
public:
  virtual bool is_level_enabled(LogLevel level) const {
    return true;
  }

  virtual void Submit(const LogRecord&) = 0;

protected:
  ~LogSink() = default;
};

typedef std::shared_ptr<LogSink> LogSinkPtr;

}  // namespace cpplog
