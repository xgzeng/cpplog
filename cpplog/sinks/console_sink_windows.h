#pragma once
#include <cpplog/config.h>
#include <cpplog/sink.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <iostream>
#include <iomanip>

namespace cpplog {

CPPLOG_INLINE char LevelLetter(LogLevel l) {
  switch (l) {
  case LogLevel::Trace:       return 'T';
  case LogLevel::Debug:       return 'D';
  case LogLevel::Information: return 'I';
  case LogLevel::Warning:     return 'W';
  case LogLevel::Error:       return 'E';
  case LogLevel::Fatal:       return 'F';
  default: return 'I';
  }
}

// wrap windows MultiByteToWideChar
inline int MultiByteToWideChar2(UINT CodePage, DWORD dwFlags, const std::string& ns, std::wstring& ws) {
  return MultiByteToWideChar(CodePage, dwFlags,
                             ns.data(), static_cast<int>(ns.size()),
                             const_cast<wchar_t*>(ws.data()),
                             static_cast<int>(ws.size()));
}

inline int WideCharToMultiByte2(UINT CodePage, DWORD dwFlags, const std::wstring& ws, std::string& ns) {
  return WideCharToMultiByte(CodePage, dwFlags,
                             ws.data(), static_cast<int>(ws.size()),
                             const_cast<char*>(ns.data()),
                             static_cast<int>(ns.size()),
                             nullptr, nullptr);
}

inline std::string Utf8ToMultibyteString(const std::string& s) {
  // UTF8 to WideChar
  std::wstring ws;
  auto wchar_count = MultiByteToWideChar2(CP_UTF8, 0, s, ws);
  ws.resize(wchar_count);
  wchar_count = MultiByteToWideChar2(CP_UTF8, 0, s, ws);
  ws.resize(wchar_count);

  // WideChar To local codepage
  std::string result;
  auto byte_count = WideCharToMultiByte2(CP_ACP, 0, ws, result);
  result.resize(byte_count);
  byte_count = WideCharToMultiByte2(CP_ACP, 0, ws, result);
  result.resize(byte_count);
  return result;
}

class ConsoleSinkWindows : public LogSink {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
public:
  ConsoleSinkWindows() {
  }

  void SubmitRecord(const LogRecord& r) override {
    struct tm tm_local {};
    localtime_s(&tm_local, &r.timestamp().tv_sec);

    const char* base_filename = strrchr(r.file_name(), '/');
    if (!base_filename) base_filename = strrchr(r.file_name(), '\\');
    base_filename = base_filename ? base_filename + 1 : r.file_name();

    // change line color
    WORD fg_color = 0;
    switch (r.level()) {
    case LogLevel::Warning:
      fg_color = FOREGROUND_RED;
      break;
    case LogLevel::Error:
    case LogLevel::Fatal:
      fg_color = FOREGROUND_RED | FOREGROUND_INTENSITY;
      break;
    case LogLevel::Information:
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
      << Utf8ToMultibyteString(r.message());

    std::cout << std::endl;
  }
};

} // namespace cpplog
