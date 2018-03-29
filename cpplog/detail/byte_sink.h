#pragma once
#include <cstddef>

namespace cpplog {

class ByteSink {
public:
  ByteSink() = default;
  virtual ~ByteSink() = default;

  virtual void Append(const void* bytes, std::size_t n) = 0;
};

} // cpplog::detail