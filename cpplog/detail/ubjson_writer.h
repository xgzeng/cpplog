#pragma once

#include "cpplog/config.h"
#include "cpplog/detail/byte_sink.h"

#include <limits> // numeric_limits
#include <cassert>

#if defined(__GNUC__) || defined(__clang__)

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  #define  TO_BIG_ENDIAN_16(value) value
  #define  TO_BIG_ENDIAN_32(value) value
  #define  TO_BIG_ENDIAN_64(value) value
#else
  #define  TO_BIG_ENDIAN_16(value) __builtin_bswap16(value)
  #define  TO_BIG_ENDIAN_32(value) __builtin_bswap32(value)
  #define  TO_BIG_ENDIAN_64(value) __builtin_bswap64(value)
#endif

#elif defined(_MSC_VER) && defined(_WIN32)

// Windows Platform is always little endian now
inline void CHECK_TYPE_SIZE() {
  static_assert(sizeof(unsigned long) == 4 && sizeof(unsigned short) == 2, "");
}

#define  TO_BIG_ENDIAN_16(value) _byteswap_ushort(value)
#define  TO_BIG_ENDIAN_32(value) _byteswap_ulong(value)
#define  TO_BIG_ENDIAN_64(value) _byteswap_uint64(value)

#else
#error "compiler not supported yet"
#endif

namespace cpplog { namespace detail {

/// integer cast safeness template
template<typename FROM, typename TO>
struct is_safe_integer_cast: std::conditional<
  // condition
  (std::is_signed<FROM>::value == std::is_signed<TO>::value
    && sizeof(FROM) <= sizeof(TO))
  || (std::is_signed<TO>::value && sizeof(FROM) < sizeof(TO)),
  // result type
  std::true_type,
  std::false_type>::type
{
};

template<typename T>
struct is_small_int {
  static const bool value = std::numeric_limits<T>::is_integer
      && !std::is_same<T, bool>::value
      && ((sizeof(T) < sizeof(int32_t) ||
        (sizeof(T) == sizeof(int32_t) && std::numeric_limits<T>::is_signed)));
};

template<typename T>
struct is_big_int {
  static const bool value = std::numeric_limits<T>::is_integer
      && !std::is_same<T, bool>::value
      && sizeof(T) > sizeof(int32_t)
      && ((sizeof(T) < sizeof(int64_t) ||
         (sizeof(T) == sizeof(int64_t) && std::numeric_limits<T>::is_signed)));
};

// template<typename T>
// struct maybe_super_int {
//   static const bool value = std::numeric_limits<T>::is_integer
//       && !std::is_same<T, bool>::value
//       && sizeof(T) > sizeof(int64_t)
//       && ((sizeof(T) < sizeof(int64_t) ||
//          (sizeof(T) == sizeof(int64_t) && std::numeric_limits<T>::is_signed)));
// };

} // namespace detail

class UBJsonWriter {
  static_assert(detail::is_small_int<int>::value, "");
  static_assert(!detail::is_small_int<unsigned int>::value, "");

public:
  UBJsonWriter(ByteSink& bs)
  : byte_sink_(bs) {
  }

  // void Flush();

  // low level operation
  UBJsonWriter& StartObject() {
    return WriteMarker('{');
  }

  UBJsonWriter& EndObject() {
    return WriteMarker('}');
  }

  UBJsonWriter& StartList() {
    return WriteMarker('[');
  }

  UBJsonWriter& EndList() {
    return WriteMarker(']');
  }

  UBJsonWriter& WriteName(string_view name);

  UBJsonWriter& WriteString(string_view value);

  UBJsonWriter& WriteBool(bool value) {
    return WriteMarker(value ? 'T' : 'F');
  }

  UBJsonWriter& WriteChar(char value) {
    return WriteMarkerAndData('C', &value, 1);
  }

  UBJsonWriter& WriteInt8(int8_t value) {
    return WriteMarkerAndData('i', &value, 1);
  }

  UBJsonWriter& WriteUInt8(uint8_t value){
    return WriteMarkerAndData('U', &value, 1);
  }

  UBJsonWriter& WriteInt16(int16_t value) {
    value = TO_BIG_ENDIAN_16(value);
    return WriteMarkerAndData('I', &value, 2);
  }

  UBJsonWriter& WriteInt32(int32_t value) {
    value = TO_BIG_ENDIAN_32(value);
    return WriteMarkerAndData('l', &value, 4);
  }

  UBJsonWriter& WriteInt64(int64_t value) {
    value = TO_BIG_ENDIAN_64(value);
    return WriteMarkerAndData('L', &value, 8);
  }

  UBJsonWriter& WriteFloat(float value) {
    static_assert(sizeof(float) == 4, "float type is not 4 bytes");
    return WriteMarkerAndData('d', &value, 4);
  }

  UBJsonWriter& WriteDouble(double value) {
    static_assert(sizeof(double) == 8, "double type is not 4 bytes");
    return WriteMarkerAndData('D', &value, 8);
  }

  UBJsonWriter& WriteNull() {
    return WriteMarker('Z');
  }

  // untyped WriteValue
  UBJsonWriter& WriteValue(string_view value) {
    return WriteString(value);
  }
  
  // note: use enable_if to disable implict convert
  //    some types (such as pointer) to bool type
  template<typename T>
  UBJsonWriter& WriteValue(T value,
      typename std::enable_if<std::is_same<T, bool>::value, void*>::type = 0) {
    return WriteBool(value);
  }

  UBJsonWriter& WriteValue(std::nullptr_t) {
    return WriteNull();
  }

  template<typename T>
  UBJsonWriter& WriteValue(T value,
      typename std::enable_if<detail::is_small_int<T>::value, void*>::type = 0) {
    static_assert(detail::is_safe_integer_cast<T, int32_t>::value, "");

    if (value >= 0) {
      if (value <= std::numeric_limits<int8_t>::max()) {
        return WriteInt8(value);
      } else if (value <= std::numeric_limits<uint8_t>::max()) {
        return WriteUInt8(value);
      } else if (value <= std::numeric_limits<int16_t>::max()) {
        return WriteInt16(value);
      } else {
        return WriteInt32(value);
      }
    } else {
      if (value >= std::numeric_limits<int8_t>::min()) {
        return WriteInt8(value);
      } else if (value >= std::numeric_limits<int16_t>::min()) {
        return WriteInt16(value);
      } else {
        return WriteInt32(value);
      }
    }    
  }

  template<typename T>
  UBJsonWriter& WriteValue(T value,
      typename std::enable_if<detail::is_big_int<T>::value, void*>::type = 0) {
    static_assert(detail::is_safe_integer_cast<T, int64_t>::value, "");

    if (value <= std::numeric_limits<int32_t>::max()) {
      return WriteValue(static_cast<int32_t>(value));
    } else {
      return WriteInt64(value);
    }
  }

  UBJsonWriter& WriteValue(uint64_t value) {
    if (value <= std::numeric_limits<int64_t>::max()) {
      return WriteValue(static_cast<int64_t>(value));
    } else {
      // TODO: support ubjson big number
      assert(false && "unsupported big number");
      return *this;
    }
  }

  UBJsonWriter& StartSubObject(string_view name) {
    WriteName(name);
    return StartObject();
  }

  UBJsonWriter& StartSubList(string_view name) {
    WriteName(name);
    return StartList();
  }

  template<typename T>
  UBJsonWriter& WriteNameValue(string_view name, T&& value) {
    WriteName(name);
    return WriteValue(std::forward<T>(value));
  }

private:
  UBJsonWriter& WriteMarker(char c);

  UBJsonWriter& WriteMarkerAndData(char c, const void* bytes, size_t byte_count);

  ByteSink& byte_sink_;
};

CPPLOG_INLINE UBJsonWriter& UBJsonWriter::WriteMarker(char c) {
  byte_sink_.Append(&c, 1);
  return *this;
}

CPPLOG_INLINE UBJsonWriter& UBJsonWriter::WriteMarkerAndData(char c,
    const void* bytes, size_t byte_count) {
  WriteMarker(c);
  byte_sink_.Append(bytes, byte_count);
  return *this;
}

CPPLOG_INLINE UBJsonWriter& UBJsonWriter::WriteName(string_view name) {
  WriteValue(name.size());
  byte_sink_.Append(name.data(), name.size());
  return *this;
}

CPPLOG_INLINE UBJsonWriter& UBJsonWriter::WriteString(string_view value) {
  WriteMarker('S');
  WriteValue(value.size());
  byte_sink_.Append(value.data(), value.size());
  return *this;
}

} // namespace cpplog::detail
