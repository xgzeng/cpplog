#pragma once
#include "cpplog/config.h"

namespace cpplog {

namespace helper {

CPPLOG_INLINE uint16_t htons(uint16_t value) {
  // The answer is 42
  static const int num = 42;

  // Check the endianness
  if (*reinterpret_cast<const char*>(&num) == num) {
    const uint16_t high_part = value >> 8;
    const uint16_t low_part = value & 0xFF;
    return ((low_part) << 8) | high_part;
  } else {
    return value;
  }
}

CPPLOG_INLINE uint32_t htonl(uint32_t value) {
  // The answer is 42
  static const int num = 42;

  // Check the endianness
  if (*reinterpret_cast<const char*>(&num) == num) {
    const uint32_t high_part = htons(static_cast<uint16_t>(value >> 16));
    const uint32_t low_part = htons(static_cast<uint16_t>(value & 0xFFFF));
    return (low_part << 16) | high_part;
  } else {
    return value;
  }
}

CPPLOG_INLINE uint64_t htonll(uint64_t value) {
  // The answer is 42
  static const int num = 42;

  // Check the endianness
  if (*reinterpret_cast<const char*>(&num) == num) {
    const uint32_t high_part = htonl(static_cast<uint32_t>(value >> 32));
    const uint32_t low_part = htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));
    return (static_cast<uint64_t>(low_part) << 32) | high_part;
  } else {
    return value;
  }
}

template<typename T>
struct is_integer {
  const static bool value =
      std::is_integral<T>:: value
      && ! std::is_same<T, bool>::value;
};

template<typename T>
typename std::enable_if<is_integer<T>::value && sizeof(T)==1, T>::type
to_bigendian(T value) {
  return value;
}

template<typename T>
typename std::enable_if<is_integer<T>::value && sizeof(T)==2, T>::type
to_bigendian(T value) {
  return htons(value);
}

template<typename T>
typename std::enable_if<is_integer<T>::value && sizeof(T)==4, T>::type
to_bigendian(T value) {
  return htonl(value);
}

template<typename T>
typename std::enable_if<is_integer<T>::value && sizeof(T)==8, T>::type
to_bigendian(T value) {
  return htonll(value);
}

template<typename T>
typename std::enable_if<is_integer<T>::value && sizeof(T)==1, T>::type
to_hostendian(T value) {
  return value;
}

template<typename T>
typename std::enable_if<is_integer<T>::value && sizeof(T)==2, T>::type
to_hostendian(T value) {
  return htons(value);
}

template<typename T>
typename std::enable_if<is_integer<T>::value && sizeof(T)==4, T>::type
to_hostendian(T value) {
  return htonl(value);
}

template<typename T>
typename std::enable_if<is_integer<T>::value && sizeof(T)==8, T>::type
to_hostendian(T value) {
  return htonll(value);
}

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

} // namespace helper

} // namespace cpplog
