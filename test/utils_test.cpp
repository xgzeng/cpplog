#include "cpplog/utils.h"
#include "catch.hpp"

using namespace cpplog::helper;

TEST_CASE("Test hton*") {
  uint16_t value = 0x1234;
  REQUIRE(htons(value) == 0x3412);

  REQUIRE(htonl(0xFEDCBA90) == 0x90BADCFE);

  REQUIRE(htonll(0xFEDCBA9012345678) == 0x7856341290BADCFE);
}

///
TEST_CASE("test is_safe_integer_cast<> template") {
  // unsigned to unsigned
  static_assert(is_safe_integer_cast<uint8_t, uint8_t>::value, "");
  static_assert(is_safe_integer_cast<uint8_t, uint16_t>::value, "");
  static_assert(is_safe_integer_cast<uint8_t, uint32_t>::value, "");
  static_assert(is_safe_integer_cast<uint8_t, uint64_t>::value, "");

  static_assert(!is_safe_integer_cast<uint16_t, uint8_t>::value, "");
  static_assert( is_safe_integer_cast<uint16_t, uint16_t>::value, "");
  static_assert( is_safe_integer_cast<uint16_t, uint32_t>::value, "");
  static_assert( is_safe_integer_cast<uint16_t, uint64_t>::value, "");

  static_assert(!is_safe_integer_cast<uint32_t, uint8_t>::value, "");
  static_assert(!is_safe_integer_cast<uint32_t, uint16_t>::value, "");
  static_assert( is_safe_integer_cast<uint32_t, uint32_t>::value, "");
  static_assert( is_safe_integer_cast<uint64_t, uint64_t>::value, "");

  static_assert(!is_safe_integer_cast<uint64_t, uint8_t>::value, "");
  static_assert(!is_safe_integer_cast<uint64_t, uint16_t>::value, "");
  static_assert(!is_safe_integer_cast<uint64_t, uint32_t>::value, "");
  static_assert( is_safe_integer_cast<uint64_t, uint64_t>::value, "");

  // signed to signed
  static_assert(is_safe_integer_cast<int8_t, int8_t>::value, "");
  static_assert(is_safe_integer_cast<int8_t, int16_t>::value, "");
  static_assert(is_safe_integer_cast<int8_t, int32_t>::value, "");
  static_assert(is_safe_integer_cast<int8_t, int64_t>::value, "");

  static_assert(!is_safe_integer_cast<int16_t, int8_t>::value, "");
  static_assert( is_safe_integer_cast<int16_t, int16_t>::value, "");
  static_assert( is_safe_integer_cast<int16_t, int32_t>::value, "");
  static_assert( is_safe_integer_cast<int16_t, int64_t>::value, "");

  static_assert(!is_safe_integer_cast<int32_t, int8_t>::value, "");
  static_assert(!is_safe_integer_cast<int32_t, int16_t>::value, "");
  static_assert( is_safe_integer_cast<int32_t, int32_t>::value, "");
  static_assert( is_safe_integer_cast<int32_t, int64_t>::value, "");

  static_assert(!is_safe_integer_cast<int64_t, int8_t>::value, "");
  static_assert(!is_safe_integer_cast<int64_t, int16_t>::value, "");
  static_assert(!is_safe_integer_cast<int64_t, int32_t>::value, "");
  static_assert( is_safe_integer_cast<int64_t, int64_t>::value, "");


  // unsigned to signed
  // unsigned type is safe to cast to signed type, if type range is samller
  static_assert(!is_safe_integer_cast<uint8_t, int8_t>::value, "");
  static_assert( is_safe_integer_cast<uint8_t, int16_t>::value, "");
  static_assert( is_safe_integer_cast<uint8_t, int32_t>::value, "");
  static_assert( is_safe_integer_cast<uint8_t, int64_t>::value, "");

  static_assert(!is_safe_integer_cast<uint16_t, int8_t>::value, "");
  static_assert(!is_safe_integer_cast<uint16_t, int16_t>::value, "");
  static_assert( is_safe_integer_cast<uint16_t, int32_t>::value, "");
  static_assert( is_safe_integer_cast<uint16_t, int64_t>::value, "");

  static_assert(!is_safe_integer_cast<uint32_t, int8_t>::value, "");
  static_assert(!is_safe_integer_cast<uint32_t, int16_t>::value, "");
  static_assert(!is_safe_integer_cast<uint32_t, int32_t>::value, "");
  static_assert( is_safe_integer_cast<uint32_t, int64_t>::value, "");

  static_assert(!is_safe_integer_cast<uint64_t, int8_t>::value, "");
  static_assert(!is_safe_integer_cast<uint64_t, int16_t>::value, "");
  static_assert(!is_safe_integer_cast<uint64_t, int32_t>::value, "");
  static_assert(!is_safe_integer_cast<uint64_t, int64_t>::value, "");

  // signed to unsigned
  // signed type is not safe to cast to signed type
  static_assert(!is_safe_integer_cast<int8_t, uint64_t>::value, "");
}

