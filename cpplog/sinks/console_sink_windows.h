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

// wrap windows MultiByteToWideChar
inline int MultiByteToWideChar2(UINT CodePage, DWORD dwFlags,
                                const std::string& ns,
                                std::wstring& ws) {
  return MultiByteToWideChar(CodePage, dwFlags,
                             ns.data(), static_cast<int>(ns.size()),
                             const_cast<wchar_t*>(ws.data()),
                             static_cast<int>(ws.size()));
}

inline int WideCharToMultiByte2(UINT CodePage, DWORD dwFlags,
                                const std::wstring& ws,
                                std::string& ns) {
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
public:
  ConsoleSinkWindows()
  : hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {
  }

  void Submit(const LogRecord& r) override {
    std::lock_guard<std::mutex> guard(mutex_);
    struct tm tm_local {};
    localtime_s(&tm_local, &r.timestamp().tv_sec);

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
    std::cout << Utf8ToMultibyteString(FormatAsText(r)) << std::endl;
  }

private:
  HANDLE hConsole;
  std::mutex mutex_;
};

} // namespace cpplog
