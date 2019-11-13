#pragma once
#include <cpplog/config.h>

// help function to convert bytes to hex string

namespace cpplog {

static const char HEX_CHARS[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

template <typename C> CPPLOG_INLINE std::string hexify(const C &bytes) {
  std::string result;
  for (char c : bytes) {
    if (!result.empty())
      result += " ";
    result += HEX_CHARS[(c & 0xF0) >> 4];
    result += HEX_CHARS[c & 0x0F];
  }
  return result;
}

CPPLOG_INLINE std::string hexify(const void* pdata, std::size_t byte_count) {
  std::string result;
  const char * pc = (const char*) pdata;
  for (int i = 0; i < byte_count; ++i) {
    char c = pc[i];
    result += HEX_CHARS[(c & 0xF0) >> 4];
    result += HEX_CHARS[c & 0x0F];
  }
  return result;
}

} // namespace cpplog
