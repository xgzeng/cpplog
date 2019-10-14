#pragma once

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

constexpr auto LVL2_TRACE = spdlog::level::level_enum::trace;
constexpr auto LVL2_DEBUG = spdlog::level::level_enum::debug;
constexpr auto LVL2_INFO  = spdlog::level::level_enum::info;
constexpr auto LVL2_WARNING  = spdlog::level::level_enum::warn;
constexpr auto LVL2_ERROR  = spdlog::level::level_enum::err;
constexpr auto LVL2_FATAL = spdlog::level::level_enum::critical;

#define LOG_TO_IMPL(logger, ...) SPDLOG_LOGGER_CALL(logger, __VA_ARGS__)

#define LOG_TO(sink, level, fmt, ...) \
    LOG_TO_IMPL(sink, LVL2_##level, fmt, ##__VA_ARGS__)

#define LOG(level, fmt, ...) \
  LOG_TO_IMPL(spdlog::default_logger_raw(), LVL2_##level, fmt, ##__VA_ARGS__)

// #define LOG_TO_IF(sink, level, condition,  fmt, ...) \
//   if (condition) LOG_TO_IMPL(sink, LVL_##level, fmt, ##__VA_ARGS__)

#define LOG_IF(level, condition, fmt, ...) \
  if (condition) LOG_TO_IMPL(spdlog::default_logger_raw(), LVL2_##level, fmt, ##__VA_ARGS__)

#define LOG_EVERY_N(level, n, fmt, ...)                          \
    static int LOG_OCCURRENCES = 0, LOG_OCCURRENCES_MOD_N = 0;   \
    ++LOG_OCCURRENCES;                                           \
    if (++LOG_OCCURRENCES_MOD_N > n) LOG_OCCURRENCES_MOD_N -= n; \
    if (LOG_OCCURRENCES_MOD_N == 1)                              \
        LOG_TO_IMPL(spdlog::default_logger_raw(), LVL2_##level, fmt, ##__VA_ARGS__)
