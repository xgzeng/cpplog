#pragma once

// #include <spdlog/fmt/bin_to_hex.h>
// help function to convert bytes to hex string

namespace cpplog {

static const char HEX_CHARS[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

template <typename C> inline std::string hexify(const C &bytes) {
  std::string result;
  for (char c : bytes) {
    result += HEX_CHARS[(c & 0xF0) >> 4];
    result += HEX_CHARS[c & 0x0F];
    result += " ";
  }
  return result;
  // return spdlog::to_hex(bytes);
}

inline std::string hexify(const void* pdata, std::size_t byte_count) {
  std::string result;
  const char * pc = (const char*) pdata;
  for (int i = 0; i < byte_count; ++i) {
    char c = pc[i];
    result += HEX_CHARS[(c & 0xF0) >> 4];
    result += HEX_CHARS[c & 0x0F];
    result += " ";
  }
  return result;
}

} // namespace cpplog
