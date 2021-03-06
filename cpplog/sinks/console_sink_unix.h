#pragma once

#include "cpplog/config.h"
#include "cpplog/sink.h"
#include "cpplog/formatter/text_formatter.h"

#include <iostream>
#include <iomanip> // std::setw etc.

namespace cpplog {

class ConsoleSinkUnix : public LogSink {
public:
  void Submit(const LogRecord& r) override {
    std::lock_guard<std::mutex> guard(mutex_);
    std::cout << "\033[0;3" << (int)SeverityColor(r.level()) << 'm'
        << FormatAsText(r) << "\033[m" << std::endl;
  }

private:
  std::mutex mutex_;

  enum DisplayColor {
    BLACK     = 0,
    RED       = 1,
    GREEN     = 2,
    YELLOW    = 3,
    BLUE      = 4,
    MAGENTA   = 5,
    CYAN      = 6,
    WHITE     = 7
  };

  static DisplayColor SeverityColor(LogLevel level) {
    switch (level) {
    case LogLevel::Debug:           return BLUE;
    case LogLevel::Warning:         return YELLOW;
    case LogLevel::Error:           return CYAN;
    case LogLevel::Fatal:           return RED;
    case LogLevel::Information:     return WHITE;
    default:                        return GREEN;
   }
  }
};

} // namespace cpplog
