#pragma once

#include "cpplog/config.h"
#include "cpplog/utils.h"
#include <string>
#include <cassert>

namespace cpplog {

namespace helper {

template<typename Writer, typename T>
CPPLOG_INLINE void msgpack_append_fmt_and_integer(Writer&& writer, uint8_t fmt, T int_value) {
  int_value = to_bigendian(int_value);
  writer.WriteByte(fmt)
        .WriteBytes(&int_value, sizeof(int_value));
}

#if 0
/// code not used
template<typename T>
struct IntegerFormatCode;

#define DEFINE_INTEGER_FMT_CODE(TYPE, CODE)   \
  template<> struct IntegerFormatCode<TYPE> { \
    const static uint8_t fmt_code = CODE;     \
  };

DEFINE_INTEGER_FMT_CODE(uint8_t,  0xcc);
DEFINE_INTEGER_FMT_CODE(uint16_t, 0xcd);
DEFINE_INTEGER_FMT_CODE(uint32_t, 0xce);
DEFINE_INTEGER_FMT_CODE(uint64_t, 0xcf);

DEFINE_INTEGER_FMT_CODE(int8_t,   0xd0);
DEFINE_INTEGER_FMT_CODE(int16_t,  0xd1);
DEFINE_INTEGER_FMT_CODE(int32_t,  0xd2);
DEFINE_INTEGER_FMT_CODE(int64_t,  0xd3);
#endif

} // helper

class StringBufferWriter {
public:
  StringBufferWriter(std::string& b) : buf_(b) {}

  StringBufferWriter& WriteByte(uint8_t b) {
    buf_.push_back(b);
    return *this;
  }

  StringBufferWriter& WriteBytes(const void* data, size_t count) {
    buf_.append((const char*)data, count);
    return *this;
  }

private:
  std::string& buf_;
};

/// float
template<typename Writer>
CPPLOG_INLINE void msgpack_pack_float(Writer&& writer, float value) {
  static_assert(sizeof(float) == sizeof(uint32_t), "");
  helper::msgpack_append_fmt_and_integer(writer, 0xca,
          *reinterpret_cast<uint32_t*>(&value));
}

template<typename Writer>
CPPLOG_INLINE void msgpack_pack_double(Writer&& writer, double value) {
  static_assert(sizeof(double) == sizeof(uint64_t), "");
  helper::msgpack_append_fmt_and_integer(writer, 0xcb,
          *reinterpret_cast<uint64_t*>(&value));
}

/// str format family
template<typename Writer>
CPPLOG_INLINE void msgpack_pack_str(Writer&& writer, string_view str) {
  if (str.length() <= 31) {
    // fix str
    writer.WriteByte(0xa0 | (str.length() & 0x1F));
  } else if (str.length() <= 255) {
    // str 8
    helper::msgpack_append_fmt_and_integer(writer, 0xd9, static_cast<uint8_t>(str.length()));
  } else if (str.length() <= 0xFFFF) {
    // str 16
    helper::msgpack_append_fmt_and_integer(writer, 0xda, static_cast<uint16_t>(str.length()));
  } else if (str.length() <= 0xFFFFFFFF){
    // str 32
    helper::msgpack_append_fmt_and_integer(writer, 0xdb, static_cast<uint32_t>(str.length()));
  }

  writer.WriteBytes(str.begin(), str.size());
}

/// array format family
template<typename Writer>
CPPLOG_INLINE void msgpack_pack_array(Writer&& writer, uint32_t array_size) {
  if (array_size <= 15) {
    // fix array
    writer.WriteByte(0x90 | (array_size & 0x0F));
  } else if (array_size <= 0xFFFF) {
    // array 16
    helper::msgpack_append_fmt_and_integer(writer, 0xdc, static_cast<uint16_t>(array_size));
  } else if (array_size <= 0xFFFFFFFF) {
    // array 32
    helper::msgpack_append_fmt_and_integer(writer, 0xdd, static_cast<uint32_t>(array_size));
  }
}

/// map format family
template<typename Writer>
CPPLOG_INLINE void msgpack_pack_map(Writer&& writer, uint32_t map_size) {
  if (map_size <= 15) {
    // fix map
    writer.WriteByte(0x80 | (map_size & 0x0F));
  } else if (map_size <= 0xFFFF) {
    // map 16
    helper::msgpack_append_fmt_and_integer(writer, 0xde, static_cast<uint16_t>(map_size));
  } else if (map_size <= 0xFFFFFFFF) {
    // map 32
    helper::msgpack_append_fmt_and_integer(writer, 0xdf, static_cast<uint32_t>(map_size));
  }
}

template<typename Writer>
CPPLOG_INLINE void msgpack_pack_nil(Writer&& writer) {
  writer.WriteByte(0xc0);
}

template<typename Writer>
CPPLOG_INLINE void msgpack_pack_boolean(Writer&& writer, bool value) {
  if (value) {
    writer.WriteByte(0xc3);
  } else {
    writer.WriteByte(0xc2);
  }
}

template<typename Writer>
CPPLOG_INLINE void msgpack_pack_int_impl32(Writer&& writer, int32_t value) {
  if ((value & ~0x7f) == 0) { // positive fixint
    writer.WriteByte(value & 0x7F);
  } else if ((value | 0x1F) == -1) { // negative fixint
    writer.WriteByte(value & 0xFF);
  } else if (value >= std::numeric_limits<int8_t>::min()
             && value <= std::numeric_limits<int8_t>::max()) { // 8bit signed int
    helper::msgpack_append_fmt_and_integer(writer, 0xd0, (int8_t)(value & 0xFF));
  } else if (value >= std::numeric_limits<int16_t>::min()
             && value <= std::numeric_limits<int16_t>::max()) { // 16bit signed int
    helper::msgpack_append_fmt_and_integer(writer, 0xd1, (int16_t)(value & 0xFFFF));
  } else { // 32bit signed int
    helper::msgpack_append_fmt_and_integer(writer, 0xd2, value);
  }
}

template<typename Writer>
CPPLOG_INLINE void msgpack_pack_int_impl64(Writer&& writer, int64_t value) {
  if (value >= std::numeric_limits<int>::min()
      && value <= std::numeric_limits<int>::max()) {
    msgpack_pack_int_impl32(writer, value & 0xFFFFFFFFFF);
  } else { // 64 bit signed int
    helper::msgpack_append_fmt_and_integer(writer, 0xd3, value);
  }
}

template<typename Writer>
CPPLOG_INLINE void msgpack_pack_uint_impl32(Writer&& writer, uint32_t value) {
  if ((value & ~0x7f) == 0) { // positive fixint
    writer.WriteByte(value & 0x7F);
  } else if (value <= std::numeric_limits<uint8_t>::max()) { // 8bit unsigned int
    helper::msgpack_append_fmt_and_integer(writer, 0xcc,
                                           static_cast<uint8_t>(value));
  } else if (value <= std::numeric_limits<uint16_t>::max()) { // 16bit unsigned int
    helper::msgpack_append_fmt_and_integer(writer, 0xcd,
                                           static_cast<uint16_t>(value));
  } else {
    helper::msgpack_append_fmt_and_integer(writer, 0xce, value);
  }
}

template<typename Writer>
CPPLOG_INLINE void msgpack_pack_uint_impl64(Writer&& writer, uint64_t value) {
  if (value <= std::numeric_limits<uint32_t>::max()) {
    msgpack_pack_uint_impl32(writer, value);
  } else {
    // 64bit uint
    helper::msgpack_append_fmt_and_integer(writer, 0xcf, value);
  }
}

/// msgpack_pack_int function
template<typename Writer, typename T>
CPPLOG_INLINE
typename std::enable_if<
  // condition
  std::is_signed<T>::value
  && helper::is_safe_integer_cast<T, int>::value,
  // return type
  void>::type
  msgpack_pack_int(Writer& writer, T value) {
  if (sizeof(int) == sizeof(int32_t)) {
    msgpack_pack_int_impl32(writer, value);
  } else if (sizeof(int) == sizeof(int64_t)) {
    msgpack_pack_int_impl64(writer, value);
  }
}

template<typename Writer, typename T>
CPPLOG_INLINE
typename std::enable_if<
  // condition
  std::is_signed<T>::value
  && !helper::is_safe_integer_cast<T, int>::value
  && helper::is_safe_integer_cast<T, int64_t>::value,
  // return type
  void>::type
  msgpack_pack_int(Writer& writer, T value) {
  msgpack_pack_int_impl64(writer, value);
}

template<typename Writer, typename T>
CPPLOG_INLINE
typename std::enable_if<
  // condition
  std::is_unsigned<T>::value
  && helper::is_safe_integer_cast<T, unsigned int>::value,
  // return type
  void>::type
  msgpack_pack_int(Writer& writer, T value) {
  if (sizeof(unsigned int) == sizeof(uint32_t)) {
    msgpack_pack_uint_impl32(writer, value);
  } else if (sizeof(unsigned int) == sizeof(uint64_t)) {
    msgpack_pack_int_impl64(writer, value);
  }
}

template<typename Writer, typename T>
CPPLOG_INLINE
typename std::enable_if<
  // condition
  std::is_unsigned<T>::value
  && !helper::is_safe_integer_cast<T, unsigned int>::value
  && helper::is_safe_integer_cast<T, uint64_t>::value,
  // return type
  void>::type
  msgpack_pack_int(Writer& writer,  T value) {
  msgpack_pack_uint_impl64(writer, value);
}

} // namespace cpplog
