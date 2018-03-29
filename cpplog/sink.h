#pragma once

#include "cpplog/config.h"
#include "cpplog/record.h"

namespace cpplog {

class LogSink {
public:
  virtual bool is_level_enabled(LogLevel) const {
    return true;
  }

  virtual void Submit(const LogRecord&) = 0;

protected:
  ~LogSink() = default;
};

}  // namespace cpplog
