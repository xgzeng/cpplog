#include "cpplog/detail/ubjson_writer.h"
#include "cpplog/detail/sstream_byte_sink.h"

#include <nlohmann/json.hpp>

#define CATCH_CONFIG_MAIN
#include "test/catch.hpp"

using namespace cpplog;

TEST_CASE("UBJsonWriter Write Explicit Types") {
  StringStreamByteSink sink;
  UBJsonWriter writer(sink);

  SECTION("Null value") {
    writer.WriteNull();
    REQUIRE(sink.str() == "Z");
  }

  SECTION("Boolean Types") {
    writer.WriteBool(true);
    writer.WriteBool(false);
    
    REQUIRE(sink.str() == "TF");
  }

  SECTION("Numeric Types") {
    writer.WriteInt8(-1);

    writer.WriteUInt8(2);

    writer.WriteInt16(-1);

    writer.WriteInt32(2147483647); // int 
    
    writer.WriteInt64(-2); // int64

    // unsigned int64

    REQUIRE(sink.str() == "i\xFFU\x2I\xFF\xFF"
        "l\x7F\xFF\xFF\xFF"
        "L\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFE");
  }

  SECTION("Char Types") {
    writer.WriteChar('A');

    REQUIRE(sink.str() == "CA");
  }

  SECTION("String Type") {
    writer.WriteString("abc");
    
    REQUIRE(sink.str() == "Si\x03" "abc");
  }
}

TEST_CASE("UBJsonWriter WriteValue") {
  StringStreamByteSink sink;
  UBJsonWriter writer(sink);

  SECTION("WriteValue null,true,false") {
    writer.WriteValue(nullptr);
    writer.WriteValue(true);
    writer.WriteValue(false);
    REQUIRE(sink.str() == "ZTF");
  }

  SECTION("WriteValue int8") {
    writer.WriteValue((int8_t)-128);
    REQUIRE(sink.str() == "i\x80");
  }

  SECTION("WriteValue uint8") {
    writer.WriteValue((uint8_t)128);
    REQUIRE(sink.str() == "U\x80");
  }

  SECTION("WriteValue int16") {
    writer.WriteValue((int16_t) 0x01FF);
    REQUIRE(sink.str() == "I\x01\xFF");
  }

  SECTION("WriteValue uint16") {
    writer.WriteValue((uint16_t) 0xFFFF);

    std::string expected{'l', '\x00', '\x00', '\xFF', '\xFF'};
    REQUIRE(sink.str() == expected);
  }

  SECTION("WriteValue int32") {
    writer.WriteValue((int32_t) 0x7FFFFFFF);
    writer.WriteValue((int32_t) -1);
    REQUIRE(sink.str() == "l\x7F\xFF\xFF\xFFi\xFF");
  }

  SECTION("WriteValue uint32") {
    writer.WriteValue((uint32_t) 0xFFFFFFFF);
    writer.WriteValue((uint32_t) 1);

    std::string expected {'L', '\x00', '\x00', '\x00', '\x00',
        '\xFF', '\xFF', '\xFF', '\xFF', 'i', '\x01'};
    
    REQUIRE(sink.str() == expected);
  }

  SECTION("WriteValue int64") {
    writer.WriteValue((int64_t) 0x1234567890ABCDEF);
    writer.WriteValue((int64_t) -1);
    REQUIRE(sink.str() == "L\x12\x34\x56\x78\x90\xAB\xCD\xEFi\xFF");
  }

  SECTION("WriteValue uint64") {
    writer.WriteValue((uint64_t) 0x7234567890ABCDEF);
    writer.WriteValue((uint64_t) 1);
    REQUIRE(sink.str() == "L\x72\x34\x56\x78\x90\xAB\xCD\xEFi\x01");
  }

  SECTION("WriteValue short/ushort") {
    writer.WriteValue((short) 1);
    writer.WriteValue((unsigned short) 1);
    REQUIRE(sink.str() == "i\x01i\x01");
  }

  SECTION("WriteValue long") {
    writer.WriteValue(1l);
    REQUIRE(sink.str() == "i\x01");
  }

  SECTION("WriteValue unsigned long") {
    writer.WriteValue(1ul);
    REQUIRE(sink.str() == "i\x01");
  }

  SECTION("WriteValue long long") {
    writer.WriteValue(1ll);
    REQUIRE(sink.str() == "i\x01");
  }

  SECTION("WriteValue unsigned long long") {
    writer.WriteValue(1ull);
    REQUIRE(sink.str() == "i\x01");
  }

  SECTION("WriteValue big number") {
    //writer.WriteValue((uint64_t) 0x8234567890ABCDEF);
    // REQUIRE(sink.str() == "L\x72\x34\x56\x78\x90\xAB\xCD\xEFi\x01");
  }
  
  SECTION("WriteValue char type") {
    writer.WriteValue('A');
    writer.WriteValue(L'A');
    //writer.WriteValue(L'汉');
    REQUIRE(sink.str() == "iAiA");
  }
}

TEST_CASE("UBJsonWriter Write Object") {
  StringStreamByteSink sink;
  UBJsonWriter writer(sink);

  SECTION("Write Empty Object") {
    writer.StartObject()
      .EndObject();
    
    REQUIRE(sink.str() == "{}");
  }

  SECTION("Write Recursive Object") {
    writer.StartObject()
        .StartSubObject("subobj")
        .EndObject()
      .EndObject();
    REQUIRE(sink.str() == "{i\x06subobj{}}");
  }
}


TEST_CASE("UBJsonWriter Write List") {
  StringStreamByteSink sink;
  UBJsonWriter writer(sink);

  SECTION("Write List") {
    writer.StartList()
      .StartList()
        .WriteValue(1)
      .EndList()
      .EndList();
    
    REQUIRE(sink.str() == "[[i\x01]]");
  }

  SECTION("Write List in Object") {
    writer.StartObject()
        .StartSubList("subobj")
        .EndList()
      .EndObject();
    REQUIRE(sink.str() == "{i\x06subobj[]}");
  }
}

TEST_CASE("UBJsonWriter Result can be parsed by nlohmann::json") {
  StringStreamByteSink sink;
  UBJsonWriter writer(sink);

  writer.StartObject()
    .StartSubObject("sub1")
      .WriteNameValue("int", 1)
      .WriteNameValue("str", "hello")
      .StartSubList("list")
        .WriteValue("world")
      .EndList()
    .EndObject()
    .EndObject();

  auto json = nlohmann::json::from_ubjson(sink.str());

  nlohmann::json expected {
    {"sub1", {
      {"int", 1},
      {"str", "hello"},
      {"list", {"world"}}
    }}
  };

  REQUIRE(json == expected);
}