#pragma once
#include "cpplog/config.h"
#include "cpplog/sink.h"
#include <iomanip> // std::setw etc.

namespace cpplog {

CPPLOG_INLINE char LevelLetter(LogLevel l) {
  switch (l) {
  case LogLevel::Trace: return 'T';
  case LogLevel::Debug: return 'D';
  case LogLevel::Information: return 'I';
  case LogLevel::Warning: return 'W';
  case LogLevel::Error: return 'E';
  case LogLevel::Fatal: return 'F';
  default: return 'I';
  }
}

class ConsoleSinkUnix : public LogSink {
public:
  void SubmitRecord(const LogRecord& r) override {
    struct tm tm_local {};
    localtime_r(&r.timestamp().tv_sec, &tm_local);

    auto& src_file_info = r.src_file_info();

    using std::setw;
    using std::setfill;
    std::cout << "\033[0;3" << (int)SeverityColor(r.level()) << 'm'
        << LevelLetter(r.level())
        << setfill('0')
        << setw(2) << 1 + tm_local.tm_mon
        << setw(2) << tm_local.tm_mday
        << ' '
        << setw(2) << tm_local.tm_hour << ':'
        << setw(2) << tm_local.tm_min << ':'
        << setw(2) << tm_local.tm_sec << '.'
        << setw(4) << (r.timestamp().tv_nsec / 1000000)
        << ' '
        << src_file_info.base_file_name() << ':' << src_file_info.line()
        << ':' << src_file_info.function_name() << "] "
        << r.message()
        << "\033[m" << std::endl;
  }

private:
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
