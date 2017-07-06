#pragma once

constexpr cpplog::LogLevel LVL_TRACE   = cpplog::LogLevel::Trace;
constexpr cpplog::LogLevel LVL_DEBUG   = cpplog::LogLevel::Debug;
constexpr cpplog::LogLevel LVL_INFO    = cpplog::LogLevel::Information;
constexpr cpplog::LogLevel LVL_WARNING = cpplog::LogLevel::Warning;
constexpr cpplog::LogLevel LVL_ERROR   = cpplog::LogLevel::Error;
constexpr cpplog::LogLevel LVL_FATAL   = cpplog::LogLevel::Fatal;

#ifdef __GNUC__
  #define __FUNCTION_SIGNATURE__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define __FUNCTION_SIGNATURE__ __FUNCSIG__
#else
  #define __FUNCTION_SIGNATURE__ ""
#endif

#define LOG_TO_IMPL(sink, level, fmt, ...) \
  if (sink.is_level_enabled(level))   \
    cpplog::LogCapture(sink, level, {__FILE__, __func__, __LINE__})  \
            .message(fmt, ##__VA_ARGS__)

#define LOG_TO(sink, level, fmt, ...) \
    LOG_TO_IMPL(sink, LVL_##level, fmt, ##__VA_ARGS__)

#define LOG(level, fmt, ...) \
  LOG_TO_IMPL(cpplog::LogDispatcher::instance(), LVL_##level, fmt, ##__VA_ARGS__)

#define LOG_TO_IF(sink, level, condition,  fmt, ...) \
  if (condition) LOG_TO_IMPL(sink, LVL_##level, fmt, ##__VA_ARGS__)

#define LOG_IF(level, condition, fmt, ...) \
  if (condition) LOG_TO_IMPL(cpplog::LogDispatcher::instance(), \
    LVL_##level, fmt, ##__VA_ARGS__)
