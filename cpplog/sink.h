#pragma once

#include "cpplog/config.h"
#include <string>
#include <time.h>
#include <vector>

namespace cpplog {

class LogRecord;

class LogSink {
public:
  virtual void SubmitRecord(LogRecord&) = 0;

protected:
  ~LogSink() = default;
};

}  // namespace cpplog

