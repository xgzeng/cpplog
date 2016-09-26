#pragma once
#include <iostream>
#include <iomanip>

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

#ifdef _WIN32
#include "console_sink_windows.h"
typedef ConsoleSinkWindows ConsoleSink;
#else
#include "console_sink_unix.h"
typedef ConsoleSinkUnix ConsoleSink;
#endif

} // namespace cpplog
