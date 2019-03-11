#pragma once

#include "cpplog/config.h"
#include "cpplog/codec/sstream_byte_sink.h"
#include "cpplog/codec/ubjson_writer.h"

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
