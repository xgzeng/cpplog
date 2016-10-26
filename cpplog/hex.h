#pragma once

// help function to convert bytes to hex string

namespace cpplog {

CPPLOG_INLINE std::string hexify(const std::string& bytes) {
  static char HEX_CHARS[] = {'0', '1', '2', '3', '4', '5', '6', '7',
	                       '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  std::string result;
  for (char c : bytes) {
	if (!result.empty()) result += " ";
	result += HEX_CHARS[(c & 0xF0) >> 4];
	result += HEX_CHARS[c & 0x0F];
  }
  return result;
}

}
