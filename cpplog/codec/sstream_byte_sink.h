#pragma once

#include "cpplog/codec/byte_sink.h"
#include <sstream>

namespace cpplog {

struct StringStreamByteSink : std::ostringstream, ByteSink {
  void Append(const void* bytes, std::size_t cnt) override {
    write(reinterpret_cast<const char*>(bytes), cnt);
  }
};

} // namespace cpplog::codec
