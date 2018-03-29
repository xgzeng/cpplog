#pragma once

#include "cpplog/config.h"
#include "cpplog/detail/sstream_byte_sink.h"
#include "cpplog/detail/ubjson_writer.h"

namespace cpplog {

class Attachment {
public:
  Attachment() {
  }

  template<typename T>
  void Attach(string_view name, T&& value) {
    writer_.WriteNameValue(name, std::forward<T>(value));
  }

private:
  StringStreamByteSink sink_;
  UBJsonWriter writer_ {sink_};
};

} // namespace cpplog
