#pragma once
#include "cpplog/config.h"
#include "cpplog/sink.h"
#include <iomanip> // std::setw etc.

namespace cpplog {

class ConsoleSinkUnix : public LogSink {
public:
  void SubmitRecord(const LogRecord& r) override {
    std::lock_guard<std::mutex> guard(mutex_);
    std::cout << "\033[0;3" << (int)SeverityColor(r.level()) << 'm'
        << FormatAsText(r) << "\033[m" << std::endl;
  }

private:
  std::mutex mutex_;
  //enum DisplayMode {
  //  RESET     = 0,  // Reset All Attributes (return to normal mode)
  //  BRIGHT    = 1,  // Bright (usually turns on BOLD)
  //  DIM       = 2,
  //  UNDERLINE = 3,
  //  BLINK     = 4,
  //  REVERSE   = 7,
  //  HIDDEN    = 8
  //};

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
