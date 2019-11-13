#pragma once

namespace cpplog {
  
class source_location {
public:
  constexpr source_location() noexcept {}
  
  constexpr source_location(const char* file_name, const char* func, int line)
  : file_name_(file_name), function_name_(func), line_(line) {
  }

  //
  constexpr const char* file_name() const noexcept {
    return file_name_;
  }

  constexpr const char* function_name() const {
    return function_name_;
  }

  constexpr std::uint_least32_t column() const noexcept {
    return column_;
  }
  
  constexpr std::uint_least32_t line() const noexcept {
    return line_;
  }

private:
  const char* file_name_ {""};
  const char* function_name_ {""};
  std::uint_least32_t line_ {0};
  std::uint_least32_t column_ {0};
};

} // namespace cpplog
