#pragma once
#include "cpplog/config.h"
#include "cpplog/sink.h"

#ifdef _WIN32
#include <cpplog/console_sink_windows.h>
#else
#include "cpplog/console_sink_unix.h"
#endif

namespace cpplog {

#ifdef _WIN32
typedef ConsoleSinkWindows ConsoleSink;
#else
typedef ConsoleSinkUnix ConsoleSink;
#endif

} // namespace cpplog
