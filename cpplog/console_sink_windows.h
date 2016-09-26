#pragma once
#include <Windows.h>

class ConsoleSinkWindows : public LogSink {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
public:
  void SubmitRecord(LogRecord& r) override {
    struct tm tm_local {};
    localtime_s(&tm_local, &r.timestamp().tv_sec);

    const char* base_filename = strrchr(r.file_name(), '/');
    if (!base_filename) base_filename = strrchr(r.file_name(), '\\');
    base_filename = base_filename ? base_filename + 1 : r.file_name();

    // change line color
    WORD fg_color = 0;
    switch (r.level()) {
    case LogLevel::warning:
      fg_color = FOREGROUND_RED;
      break;
    case LogLevel::error:
    case LogLevel::fatal:
      fg_color = FOREGROUND_RED | FOREGROUND_INTENSITY;
      break;
    case LogLevel::info:
    default:
      fg_color = FOREGROUND_GREEN;
    }

    SetConsoleTextAttribute(hConsole, fg_color);

    using std::setw;
    using std::setfill;
    std::cout << LevelLetter(r.level())
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
      << r.message();

    std::cout << std::endl;
  }
};
