#include "cpplog/msgpack_pack.h"
#include "cpplog/msgpack_unpack.h"
#include "cpplog/hex.h"

#include "catch.hpp"
#include "fakeit.hpp"

using namespace cpplog;
using namespace fakeit;

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

TEST_CASE("Test nil/bool pack") {
  Mock<msgpack_visitor> mock_visitor;

  Fake(Method(mock_visitor, on_boolean));
  Fake(Method(mock_visitor, on_nil));

  std::string buffer;
  StringBufferWriter writer(buffer);

  SECTION("msg_pack_bool(true)") {
    msgpack_pack_boolean(writer, true);
    msgpack_unpack(buffer, mock_visitor.get());
    Verify(Method(mock_visitor, on_boolean).Using(true)).Once();
  }

  SECTION("msg_pack_bool(false)") {
    msgpack_pack_boolean(writer, false);
    msgpack_unpack(buffer, mock_visitor.get());
    Verify(Method(mock_visitor, on_boolean).Using(false)).Once();
  }

  SECTION("msg_pack_nil()") {
    msgpack_pack_nil(writer);
    msgpack_unpack(buffer, mock_visitor.get());
    Verify(Method(mock_visitor, on_nil)).Once();
  }
}

#define TEST_PACK_INT(n, expected_seq)                     \
  SECTION("msg_pack_int(" STRINGIFY(n) ")") {              \
    static_assert(std::is_signed<decltype(n)>::value, ""); \
    msgpack_pack_int(writer, n);                           \
    REQUIRE(buffer == std::string expected_seq);           \
    msgpack_unpack(buffer, mock_visitor.get());            \
    Verify(Method(mock_visitor, on_int).Using(n)).Once();  \
  }

#define TEST_PACK_BIGINT(n, expected_seq)                  \
  SECTION("msg_pack_int(" STRINGIFY(n) ")") {              \
    static_assert(std::is_signed<decltype(n)>::value, ""); \
    msgpack_pack_int(writer, n);                           \
    REQUIRE(buffer == std::string expected_seq);           \
    msgpack_unpack(buffer, mock_visitor.get());            \
    Verify(Method(mock_visitor, on_longlong).Using(n)).Once();  \
  }

TEST_CASE("Test Signed Int Family Packing") {
  Mock<msgpack_visitor> mock_visitor;

  Fake(Method(mock_visitor, on_int));

  std::string buffer;
  StringBufferWriter writer(buffer);

  // positive fix int
  TEST_PACK_INT(0,    {'\x00'});
  TEST_PACK_INT(1,    {'\x01'});
  TEST_PACK_INT(127,  {'\x7f'});
  TEST_PACK_INT(std::numeric_limits<signed char>::max(),  {'\x7f'});

  // data type don't affect result
  TEST_PACK_INT((int16_t)0,  {'\x00'});

  // negative fix int
  TEST_PACK_INT(-1, {'\xff'});
  TEST_PACK_INT(-32, {'\xe0'});

  // 8bit int
  TEST_PACK_INT(-33, ({'\xD0', -33}));
  TEST_PACK_INT(-128, ({'\xD0', '\x80'}));

  // 16bit int
  TEST_PACK_INT(128,  ({'\xD1', '\x00', '\x80'}));
  TEST_PACK_INT(255,  ({'\xD1', '\x00', '\xFF'}));
  TEST_PACK_INT(256,  ({'\xD1', '\x01', '\x00'}));
  TEST_PACK_INT(32767,  ({'\xD1', '\x7F', '\xFF'}));

  TEST_PACK_INT(-129, ({'\xD1', '\xFF', '\x7F'}));
  TEST_PACK_INT(-256, ({'\xD1', '\xFF', '\x00'}));
  TEST_PACK_INT(-257, ({'\xD1', '\xFE', '\xFF'}));
  TEST_PACK_INT(-32768, ({'\xD1', '\x80', '\x00'}));

  // 32bit int
  TEST_PACK_INT(32768,  ({'\xD2', '\x00', '\x00', '\x80', '\x00'}));
  TEST_PACK_INT(65536,  ({'\xD2', '\x00', '\x01', '\x00', '\x00'}));
  TEST_PACK_INT(2147483647,  ({'\xD2', '\x7F', '\xFF', '\xFF', '\xFF'}));

  // data type don't affect result
  TEST_PACK_INT(2147483647l,  ({'\xD2', '\x7F', '\xFF', '\xFF', '\xFF'}));

  TEST_PACK_INT(-32769, ({'\xD2', '\xFF', '\xFF', '\x7F', '\xFF'}));

  // -2147483648 is treated as logn, instead of int
  TEST_PACK_INT(-2147483648, ({'\xD2', '\x80', '\x00', '\x00', '\x00'}));

  // 64bit int
  Fake(Method(mock_visitor, on_longlong));

  TEST_PACK_BIGINT(2147483648, ({'\xD3', '\x00', '\x00', '\x00', '\x00',
                                         '\x80', '\x00', '\x00', '\x00'}));
}

#define TEST_PACK_UINT(n, expected_seq)                    \
  SECTION("msg_pack_int(" STRINGIFY(n) ")") {              \
    static_assert(std::is_unsigned<decltype(n)>::value,    \
                  "unsigned value required");              \
    msgpack_pack_int(writer, n);                           \
    REQUIRE(buffer == std::string expected_seq);           \
    msgpack_unpack(buffer, mock_visitor.get());            \
    Verify(Method(mock_visitor, on_uint).Using(n)).Once();  \
  }

#define TEST_PACK_BIGUINT(n, expected_seq)                  \
  SECTION("msg_pack_int(" STRINGIFY(n) ")") {              \
    static_assert(std::is_unsigned<decltype(n)>::value,    \
                  "unsigned value required");              \
    msgpack_pack_int(writer, n);                           \
    REQUIRE(buffer == std::string expected_seq);           \
    msgpack_unpack(buffer, mock_visitor.get());            \
    Verify(Method(mock_visitor, on_ulonglong).Using(n)).Once(); \
  }

TEST_CASE("Test Unsigned Int Family Packing") {
  Mock<msgpack_visitor> mock_visitor;

  Fake(Method(mock_visitor, on_uint));

  std::string buffer;
  StringBufferWriter writer(buffer);

  // positive fix int
#if 0
  // small unsigned integer is still encoded as positiv fix int
  TEST_PACK_UINT(0u,    {'\x00'});
  TEST_PACK_UINT(1u,    {'\x01'});
  TEST_PACK_UINT(127u,  {'\x7f'});
#endif

  // 8bit unsigned int
  TEST_PACK_UINT(128u,  ({'\xcc', '\x80'}));
  TEST_PACK_UINT(255u,  ({'\xcc', '\xFF'}));

  // 16bit unsigned int
  TEST_PACK_UINT(256u,  ({'\xcd', '\x01', '\x00'}));
  TEST_PACK_UINT(65535u,  ({'\xcd', '\xFF', '\xFF'}));

  // 32bit unsigned int
  TEST_PACK_UINT(65536u,  ({'\xce', '\x00', '\x01', '\x00', '\x00'}));

  TEST_PACK_UINT(4294967295u, ({'\xce', '\xff', '\xff', '\xff', '\xff'}));

  // 64bit unsigned int
  Fake(Method(mock_visitor, on_ulonglong));
  TEST_PACK_BIGUINT(4294967296u, ({'\xcf', '\x00', '\x00', '\x00', '\x01',
                                        '\x00', '\x00', '\x00', '\x00'}));

  // data type don't affect result
  TEST_PACK_UINT(128ull,  ({'\xcc', '\x80'}));
}

TEST_CASE("Test array pack and unpack") {
  Mock<msgpack_visitor> mock_visitor;

  Fake(Method(mock_visitor, on_array));

  std::string buffer;
  StringBufferWriter writer(buffer);

#define TEST_PACK_ARRAY(array_size, expected_sequence)                 \
  SECTION("msg_pack_array(" STRINGIFY(array_size) ")") {               \
    msgpack_pack_array(writer, array_size);                            \
    REQUIRE(buffer == std::string expected_sequence);                  \
    msgpack_unpack(buffer, mock_visitor.get());                        \
    Verify(Method(mock_visitor, on_array).Using(array_size)).Once();   \
  }

  TEST_PACK_ARRAY(0, {'\x90'});

  TEST_PACK_ARRAY(15, {'\x9F'});

  TEST_PACK_ARRAY(16, ({'\xdc', '\x00', '\x10'}));

  TEST_PACK_ARRAY(65535, ({'\xdc', '\xFF', '\xFF'}));

  TEST_PACK_ARRAY(65536, ({'\xdd', '\x00', '\x01', '\x00', '\x00'}));

  TEST_PACK_ARRAY(4294967295, ({'\xdd', '\xFF', '\xFF', '\xFF', '\xFF'}));

#undef TEST_PACK_ARRAY
}

TEST_CASE("Test map pack and unpack") {
  Mock<msgpack_visitor> mock_visitor;

  Fake(Method(mock_visitor, on_map));

  std::string buffer;
  StringBufferWriter writer(buffer);

#define TEST_PACK_MAP(n, expected_sequence)                 \
  SECTION("msg_pack_array(" STRINGIFY(n) ")") {               \
    msgpack_pack_map(writer, n);                            \
    REQUIRE(buffer == std::string expected_sequence);                  \
    msgpack_unpack(buffer, mock_visitor.get());                        \
    Verify(Method(mock_visitor, on_map).Using(n)).Once();   \
  }

  TEST_PACK_MAP(0, {'\x80'});

  TEST_PACK_MAP(15, {'\x8F'});

  TEST_PACK_MAP(16, ({'\xde', '\x00', '\x10'}));

  TEST_PACK_MAP(65535, ({'\xde', '\xFF', '\xFF'}));

  TEST_PACK_MAP(65536, ({'\xdf', '\x00', '\x01', '\x00', '\x00'}));

  TEST_PACK_MAP(4294967295, ({'\xdf', '\xFF', '\xFF', '\xFF', '\xFF'}));

#undef TEST_PACK_MAP
}

TEST_CASE("Test pack string") {
  Mock<msgpack_visitor> mock_visitor;

  Fake(Method(mock_visitor, on_string));

  std::string buffer;
  StringBufferWriter writer(buffer);

#if 1
  SECTION("") {
    std::string expected_string {""};
    msgpack_pack_str(writer, "");
    REQUIRE(buffer == std::string{"\xa0"});
    msgpack_unpack(buffer, mock_visitor.get());
    Verify(Method(mock_visitor, on_string)
           .Using("")).Once();
  }
#endif

#define TEST_PACK_STRING(s, expected_seq)                 \
  SECTION("msg_pack_string(" STRINGIFY(s) ")") {          \
    std::string expected_string {expected_seq};           \
    msgpack_pack_str(writer, s);                          \
    REQUIRE(buffer == expected_string);                   \
    msgpack_unpack(buffer, mock_visitor.get());           \
    Verify(Method(mock_visitor, on_string).Using(s)).Once();    \
  }

  TEST_PACK_STRING("", "\xa0");
  TEST_PACK_STRING(std::string(31, 'a'), ("\xbf" + std::string(31, 'a')));
  // str 8
  TEST_PACK_STRING(std::string(32, 'a'), ("\xd9\x20" + std::string(32, 'a')));
  // str 16
  TEST_PACK_STRING(std::string(256, 'a'), (std::string{'\xda', '\x01', '\x00'}
                                          + std::string(256, 'a')));
  // str 32
  TEST_PACK_STRING(std::string(65536, 'a'),
    (std::string{'\xdb', '\x00', '\x01', '\x00', '\x00'} + std::string(65536, 'a')));
}

TEST_CASE("Test float pack") {
  Mock<msgpack_visitor> mock_visitor;

  Fake(Method(mock_visitor, on_float));
  Fake(Method(mock_visitor, on_double));

  std::string buffer;
  StringBufferWriter writer(buffer);

  SECTION("float type") {
    msgpack_pack_float(writer, 1.0);
    msgpack_unpack(buffer, mock_visitor.get());
    Verify(Method(mock_visitor, on_float).Using(1.0)).Once();
  }

  SECTION("float type") {
    msgpack_pack_double(writer, 0.1);
    msgpack_unpack(buffer, mock_visitor.get());
    Verify(Method(mock_visitor, on_double).Using(0.1)).Once();
  }
}

