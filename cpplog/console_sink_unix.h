#pragma once
#include <cpplog/config.h>
#include <cpplog/sink.h>

namespace cpplog {

CPPLOG_INLINE char LevelLetter(LogLevel l) {
  switch (l) {
  case LogLevel::trace: return 'T';
  case LogLevel::debug: return 'D';
  case LogLevel::info: return 'I';
  case LogLevel::warning: return 'W';
  case LogLevel::error: return 'E';
  case LogLevel::fatal: return 'F';
  default: return 'I';
  }
}

class ConsoleSinkUnix : public LogSink {
public:
  void SubmitRecord(LogRecord& r) override {
    struct tm tm_local {};
    localtime_r(&r.timestamp().tv_sec, &tm_local);

    const char* base_filename = strrchr(r.file_name(), '/');
    base_filename = base_filename ? base_filename + 1 : r.file_name();

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
        << base_filename << ':' << r.line() << ':' << r.function_name() << "] "
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
    case LogLevel::warning: return YELLOW;
    case LogLevel::error: return CYAN;
    case LogLevel::fatal: return RED;
    case LogLevel::info:
    default:
      return GREEN;
   }
  }
};

} // namespace cpplog
