#pragma once
#include <string>
#include "cpplog/utils.h"
#include "cpplog/hex.h"
#include <cassert>

namespace cpplog {

struct msgpack_visitor {
  virtual void on_int      (int       value) = 0;
  virtual void on_uint     (unsigned int value) = 0;
  virtual void on_longlong (long long value) = 0;
  virtual void on_ulonglong(unsigned long long value) = 0;

  virtual void on_float(float value) = 0;
  virtual void on_double(double value) = 0;

  virtual void on_string(string_view s) = 0;
  virtual void on_nil() = 0;
  virtual void on_boolean(bool    value) = 0;
  virtual void on_array  (uint32_t array_size) = 0;
  virtual void on_map    (uint32_t map_size) = 0;

  virtual void more_data () = 0;
};

namespace helper {

CPPLOG_INLINE string_view make_string_view(const char* begin, const char* end) {
  if (begin == end)
    return string_view();
  else
    return string_view(begin, end - begin);
};

template<typename T, typename F>
int read_integer(string_view buffer, F&& f) {
  if (buffer.size() < sizeof(T)) {
    return 0;
  }

  T value;
  memcpy(&value, buffer.data(), sizeof(T));
  value = to_hostendian(value);

  f(value);
  return sizeof(T);
}

//@return return consumed bytes, 0 means can't continue process
template<typename T>
typename std::enable_if<helper::is_safe_integer_cast<T, int>::value, int>::type
msgpack_read_signed(string_view buffer, msgpack_visitor& visitor) {
  static_assert(std::is_signed<T>::value, "");

  int byte_consumed = read_integer<T>(buffer,
          [&](T value) { visitor.on_int(value); });

  if (byte_consumed == 0) {
    visitor.more_data();
  }

  return byte_consumed;
}

template<typename T>
typename std::enable_if<!helper::is_safe_integer_cast<T, int>::value, int>::type
msgpack_read_signed(string_view buffer, msgpack_visitor& visitor) {
  static_assert(std::is_signed<T>::value, "");
  int byte_consumed = read_integer<T>(buffer,
      [&](T value) {
          if (value >= std::numeric_limits<int>::min()
              && value <= std::numeric_limits<int>::max()) {
            visitor.on_int(value);
          } else {
            visitor.on_longlong(value);
          }
      });

  if (byte_consumed == 0) {
    visitor.more_data();
  }

  return byte_consumed;
}

template<typename T>
typename std::enable_if<helper::is_safe_integer_cast<T, unsigned int>::value, int>::type
msgpack_read_unsigned(string_view buffer, msgpack_visitor& visitor) {
  static_assert(std::is_unsigned<T>::value, "");

  int byte_consumed = read_integer<T>(buffer,
          [&](T value) { visitor.on_uint(value); });

  if (byte_consumed == 0) {
    visitor.more_data();
  }

  return byte_consumed;
}

template<typename T>
typename std::enable_if<!helper::is_safe_integer_cast<T, unsigned int>::value, int>::type
msgpack_read_unsigned(string_view buffer, msgpack_visitor& visitor) {
  static_assert(std::is_unsigned<T>::value, "");
  int byte_consumed = read_integer<T>(buffer,
      [&](T value) {
          if (value <= std::numeric_limits<unsigned int>::max()) {
            visitor.on_uint(value);
          } else {
            visitor.on_ulonglong(value);
          }
      });

  if (byte_consumed == 0) {
    visitor.more_data();
  }

  return byte_consumed;
}

template<typename T, typename F>
size_t read_length_then_data(string_view buffer, F&& data_handler) {
  if (buffer.size() < sizeof(T)) {
    return 0;
  }

  T data_size;
  memcpy(&data_size, buffer.data(), sizeof(T));
  data_size = to_hostendian(data_size);

  if (buffer.size() < (sizeof(T) + data_size)) {
    return 0;
  }

  data_handler(buffer.begin() + sizeof(T), data_size);

  return sizeof(T) + data_size;
}

} // namespace helper

CPPLOG_INLINE std::pair<bool, const char*>
    msgpack_unpack_varlen(uint8_t fmt, string_view remain_buffer, msgpack_visitor& visitor) {
  int consumed_bc = 0; // consumed byte count
  if ((fmt & 0xE0) == 0xA0) { // fix str
    const size_t string_length = fmt & 0x1F;
    if (remain_buffer.size() < string_length) {
      visitor.more_data();
      return std::make_pair(false, nullptr);
    } else {
      visitor.on_string({remain_buffer.begin(), string_length});
      return std::make_pair(true, remain_buffer.begin() + (fmt & 0x1F));
    }
  }

  switch (fmt) {
  case 0xca: // float32
    consumed_bc = helper::read_integer<uint32_t>(remain_buffer,
              [&](uint32_t value) {
                  visitor.on_float(*reinterpret_cast<float*>(&value));
              });
    if (consumed_bc == 0) visitor.more_data();
    break;
  case 0xcb: // float64
    consumed_bc = helper::read_integer<uint64_t>(remain_buffer,
            [&](uint64_t value) {
                visitor.on_double(*reinterpret_cast<double*>(&value));
            });
    if (consumed_bc == 0) visitor.more_data();
    break;
  case 0xcc: // uint8
    consumed_bc = helper::msgpack_read_unsigned<uint8_t>(remain_buffer, visitor);
    break;
  case 0xcd: // uint16
    consumed_bc = helper::msgpack_read_unsigned<uint16_t>(remain_buffer, visitor);
    break;
  case 0xce: // uint32
    consumed_bc = helper::msgpack_read_unsigned<uint32_t>(remain_buffer, visitor);
    break;
  case 0xcf: // uint64
    consumed_bc = helper::msgpack_read_unsigned<uint64_t>(remain_buffer, visitor);
    break;
  case 0xd0: // int8
    consumed_bc = helper::msgpack_read_signed<int8_t>(remain_buffer, visitor);
    break;
  case 0xd1: // int16
    consumed_bc = helper::msgpack_read_signed<int16_t>(remain_buffer, visitor);
    break;
  case 0xd2: // int32
    consumed_bc = helper::msgpack_read_signed<int32_t>(remain_buffer, visitor);
    break;
  case 0xd3: // int64
    consumed_bc = helper::msgpack_read_signed<int64_t>(remain_buffer, visitor);
    break;
  case 0xd9: // str8
    consumed_bc = helper::read_length_then_data<uint8_t>(remain_buffer,
                [&](const char* data, size_t data_size) {
                  visitor.on_string({data, data_size});
                });
    if (consumed_bc == 0) visitor.more_data();
    break;
  case 0xda: // str16
    consumed_bc = helper::read_length_then_data<uint16_t>(remain_buffer,
                [&](const char* data, size_t data_size) {
                  visitor.on_string({data, data_size});
                });
    if (consumed_bc == 0) visitor.more_data();
    break;
  case 0xdb: // str32
    consumed_bc = helper::read_length_then_data<uint32_t>(remain_buffer,
                [&](const char* data, size_t data_size) {
                  visitor.on_string({data, data_size});
                });
    if (consumed_bc == 0) visitor.more_data();
    break;
  case 0xdc: // array16
    consumed_bc = helper::read_integer<uint16_t>(remain_buffer,
              [&](uint16_t value) { visitor.on_array(value); });
    if (consumed_bc == 0) visitor.more_data();
    break;
  case 0xdd: // array32
    consumed_bc = helper::read_integer<uint32_t>(remain_buffer,
              [&](uint32_t value) { visitor.on_array(value); });
    if (consumed_bc == 0) visitor.more_data();
    break;
  case 0xde: // map16
    consumed_bc = helper::read_integer<uint16_t>(remain_buffer,
              [&](uint16_t value) { visitor.on_map(value); });
    if (consumed_bc == 0) visitor.more_data();
    break;
  case 0xdf: // map32
    consumed_bc = helper::read_integer<uint32_t>(remain_buffer,
              [&](uint32_t value) { visitor.on_map(value); });
    if (consumed_bc == 0) visitor.more_data();
    break;
  default: // unimplemented
    return std::make_pair(false, nullptr);
  }

  if (consumed_bc != 0) {
    return std::make_pair(true, remain_buffer.begin() + consumed_bc);
  } else {
    return std::make_pair(false, nullptr);
  }
}

/// @return consumed byte count
CPPLOG_INLINE int msgpack_unpack(string_view buffer, msgpack_visitor& visitor) {
  auto p_fmt = buffer.begin();

  while (p_fmt != buffer.end()) {
    const uint8_t fmt = *p_fmt;
    if (fmt <= 0x7f) {  // positive fix int
      visitor.on_int(fmt & 0x7F);
      ++p_fmt;
    } else if ((fmt & 0xF0) == 0x80) { // fix map
      visitor.on_map(fmt & 0x0F);
      ++p_fmt;
    } else if ((fmt & 0xF0) == 0x90) { // fix array
      visitor.on_array(fmt & 0x0F);
      ++p_fmt;
    } else if ((fmt & 0xE0) == 0xE0) { // negative fix int
      visitor.on_int((int8_t)fmt);
      ++p_fmt;
    } else if (fmt == 0xc0) {
      visitor.on_nil();
      ++p_fmt;
    } else if (fmt == 0xc2) {
      visitor.on_boolean(false);
      ++p_fmt;
    } else if (fmt == 0xc3) {
      visitor.on_boolean(true);
      ++p_fmt;
    } else {
      // types with more data after fmt byte
      string_view remain_buffer = helper::make_string_view(p_fmt + 1, buffer.end());
      std::pair<bool, const char*> result = msgpack_unpack_varlen(fmt, remain_buffer, visitor);
      if (!result.first) {
        break; // can't continue any more
      } else {
        p_fmt ++;
        p_fmt += result.second - remain_buffer.begin();
      }
    }
  }

  return p_fmt - buffer.begin();
}

} // namespace cpplog

