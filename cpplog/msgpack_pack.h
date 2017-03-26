#pragma once

#include "cpplog/config.h"
#include "cpplog/utils.h"
#include <string>
#include <cassert>

namespace cpplog {

namespace helper {

CPPLOG_INLINE void msgpack_append_fmt(std::string& buffer, uint8_t fmt) {
  buffer.push_back(fmt);
}

CPPLOG_INLINE void msgpack_append_bytes(std::string& buffer, string_view bytes) {
  buffer.append(bytes.begin(), bytes.end());
}

template<typename T>
CPPLOG_INLINE void msgpack_append_fmt_and_integer(std::string& buffer, uint8_t fmt, T int_value) {
  buffer.push_back(fmt);
  int_value = to_bigendian(int_value);
  buffer.append((const char*) &int_value, sizeof(int_value));
}

} // helper

/// int family
CPPLOG_INLINE void msgpack_pack_uint8(std::string& buffer, uint8_t value) {
  helper::msgpack_append_fmt_and_integer(buffer, 0xcc, value);
}

CPPLOG_INLINE void msgpack_pack_uint16(std::string& buffer, uint16_t value) {
  helper::msgpack_append_fmt_and_integer(buffer, 0xcd, value);
}

CPPLOG_INLINE void msgpack_pack_uint32(std::string& buffer, uint32_t value) {
  helper::msgpack_append_fmt_and_integer(buffer, 0xce, value);
}

CPPLOG_INLINE void msgpack_pack_uint64(std::string& buffer, uint64_t value) {
  helper::msgpack_append_fmt_and_integer(buffer, 0xcf, value);
}

CPPLOG_INLINE void msgpack_pack_int8(std::string& buffer, int8_t value) {
  helper::msgpack_append_fmt_and_integer(buffer, 0xd0, value);
}

CPPLOG_INLINE void msgpack_pack_int16(std::string& buffer, int16_t value) {
  helper::msgpack_append_fmt_and_integer(buffer, 0xd1, value);
}

CPPLOG_INLINE void msgpack_pack_int32(std::string& buffer, int32_t value) {
  helper::msgpack_append_fmt_and_integer(buffer, 0xd2, value);
}

CPPLOG_INLINE void msgpack_pack_int64(std::string& buffer, int64_t value) {
  helper::msgpack_append_fmt_and_integer(buffer, 0xd3, value);
}

/// float
CPPLOG_INLINE void msgpack_pack_float(std::string& buffer, float value) {
  static_assert(sizeof(float) == sizeof(uint32_t), "");
  helper::msgpack_append_fmt_and_integer(buffer, 0xca,
          *reinterpret_cast<uint32_t*>(&value));
}

CPPLOG_INLINE void msgpack_pack_double(std::string& buffer, double value) {
  static_assert(sizeof(double) == sizeof(uint64_t), "");

  helper::msgpack_append_fmt_and_integer(buffer, 0xcb,
          *reinterpret_cast<uint64_t*>(&value));
}

/// str format family
CPPLOG_INLINE void msgpack_pack_str(std::string& buffer, string_view str) {
  if (str.length() <= 31) {
    // fix str
    helper::msgpack_append_fmt(buffer, 0xa0 | (str.length() & 0x1F));
  } else if (str.length() <= 255) {
    // str 8
    helper::msgpack_append_fmt_and_integer<uint8_t>(buffer, 0xd9, str.length());
  } else if (str.length() <= 0xFFFF) {
    // str 16
    helper::msgpack_append_fmt_and_integer<uint16_t>(buffer, 0xda, str.length());
  } else if (str.length() <= 0xFFFFFFFF){
    // str 32
    helper::msgpack_append_fmt_and_integer<uint32_t>(buffer, 0xdb, str.length());
  }
  helper::msgpack_append_bytes(buffer, str);
}

/// array format family
CPPLOG_INLINE void msgpack_pack_array(std::string& buffer, uint32_t array_size) {
  if (array_size <= 15) {
    // fix array
    helper::msgpack_append_fmt(buffer, 0x90 | (array_size & 0x0F));
  } else if (array_size <= 0xFFFF) {
    // array 16
    helper::msgpack_append_fmt_and_integer<uint16_t>(buffer, 0xdc, array_size);
  } else if (array_size <= 0xFFFFFFFF) {
    // array 32
    helper::msgpack_append_fmt_and_integer<uint32_t>(buffer, 0xdd, array_size);
  }
}

/// map format family
CPPLOG_INLINE void msgpack_pack_map(std::string& buffer, uint32_t map_size) {
  if (map_size <= 15) {
    // fix map
    helper::msgpack_append_fmt(buffer, 0x80 | (map_size & 0x0F));
  } else if (map_size <= 0xFFFF) {
    // map 16
    helper::msgpack_append_fmt_and_integer<uint16_t>(buffer, 0xde, map_size);
  } else if (map_size <= 0xFFFFFFFF) {
    // map 32
    helper::msgpack_append_fmt_and_integer<uint32_t>(buffer, 0xdf, map_size);
  }
}

CPPLOG_INLINE void msgpack_pack_nil(std::string& buffer) {
  helper::msgpack_append_fmt(buffer, 0xc0);
}

CPPLOG_INLINE void msgpack_pack_boolean(std::string& buffer, bool value) {
  if (value) {
    helper::msgpack_append_fmt(buffer, 0xc3);
  } else {
    helper::msgpack_append_fmt(buffer, 0xc2);
  }
}

CPPLOG_INLINE void msgpack_pack_int_impl32(std::string& buffer, int32_t value) {
  if ((value & ~0x7f) == 0) { // positive fixint
    helper::msgpack_append_fmt(buffer, value & 0x7F);
  } else if ((value | 0x1F) == -1) { // negative fixint
    helper::msgpack_append_fmt(buffer, value & 0xFF);
  } else if (value >= std::numeric_limits<int8_t>::min()
             && value <= std::numeric_limits<int8_t>::max()) { // 8bit signed int
    msgpack_pack_int8(buffer, value & 0xFF);
  } else if (value >= std::numeric_limits<int16_t>::min()
             && value <= std::numeric_limits<int16_t>::max()) { // 16bit signed int
    msgpack_pack_int16(buffer, value & 0xFFFF);
  } else {
    msgpack_pack_int32(buffer, value);
  }
}

CPPLOG_INLINE void msgpack_pack_int_impl64(std::string& buffer, int64_t value) {
  if (value >= std::numeric_limits<int>::min()
      && value <= std::numeric_limits<int>::max()) {
    msgpack_pack_int_impl32(buffer, value & 0xFFFFFFFFFF);
  } else {
    msgpack_pack_int64(buffer, value);
  }
}

CPPLOG_INLINE void msgpack_pack_uint_impl32(std::string& buffer, uint32_t value) {
  if ((value & ~0x7f) == 0) { // positive fixint
    helper::msgpack_append_fmt(buffer, value & 0x7F);
  } else if (value <= std::numeric_limits<uint8_t>::max()) { // 8bit unsigned int
    msgpack_pack_uint8(buffer, value);
  } else if (value <= std::numeric_limits<uint16_t>::max()) { // 16bit unsigned int
    msgpack_pack_uint16(buffer, value);
  } else {
    msgpack_pack_uint32(buffer, value);
  }
}

CPPLOG_INLINE void msgpack_pack_uint_impl64(std::string& buffer, uint64_t value) {
  if (value <= std::numeric_limits<unsigned int>::max()) { // 16bit unsigned int
    msgpack_pack_uint_impl32(buffer, value);
  } else {
    msgpack_pack_uint64(buffer, value);
  }
}

/// msgpack_pack_int function
template<typename T>
CPPLOG_INLINE
typename std::enable_if<
  // condition
  std::is_signed<T>::value
  && helper::is_safe_integer_cast<T, int>::value,
  // return type
  void>::type
  msgpack_pack_int(std::string& buffer, T value) {
  if (sizeof(int) == sizeof(int32_t)) {
    msgpack_pack_int_impl32(buffer, value);
  } else if (sizeof(int) == sizeof(int64_t)) {
    msgpack_pack_int_impl64(buffer, value);
  }
}

template<typename T>
CPPLOG_INLINE
typename std::enable_if<
  // condition
  std::is_signed<T>::value
  && !helper::is_safe_integer_cast<T, int>::value
  && helper::is_safe_integer_cast<T, int64_t>::value,
  // return type
  void>::type
  msgpack_pack_int(std::string& buffer, T value) {
  msgpack_pack_int_impl64(buffer, value);
}

template<typename T>
CPPLOG_INLINE
typename std::enable_if<
  // condition
  std::is_unsigned<T>::value
  && helper::is_safe_integer_cast<T, unsigned int>::value,
  // return type
  void>::type
  msgpack_pack_int(std::string& buffer, T value) {
  if (sizeof(unsigned int) == sizeof(uint32_t)) {
    msgpack_pack_uint_impl32(buffer, value);
  } else if (sizeof(unsigned int) == sizeof(uint64_t)) {
    assert(false);
    //msgpack_pack_int_impl64(buffer, value);
  }
}

template<typename T>
CPPLOG_INLINE
typename std::enable_if<
  // condition
  std::is_unsigned<T>::value
  && !helper::is_safe_integer_cast<T, unsigned int>::value
  && helper::is_safe_integer_cast<T, uint64_t>::value,
  // return type
  void>::type
  msgpack_pack_int(std::string& buffer, T value) {
  msgpack_pack_uint_impl64(buffer, value);
}

//CPPLOG_INLINE void msgpack_pack_int(std::string& buffer, int64_t value) {
//  msgpack_pack_int_impl64(buffer, value);
//}

} // namespace cpplog
