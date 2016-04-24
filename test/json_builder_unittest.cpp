#include "catch.hpp"
#include <cpplog/json_builder.h>

using namespace cpplog;

TEST_CASE("JsonBuilder build simple value") {
  JsonBuilder js;
  REQUIRE(js.WriteInteger(1).ExtractString() == "1");

  REQUIRE(js.WriteDouble(2.1).ExtractString() == "2.1");

  REQUIRE(js.WriteBool(true).ExtractString() == "true");

  REQUIRE(js.WriteBool(false).ExtractString() == "false");

  REQUIRE(js.WriteNull().ExtractString() == "null");

  REQUIRE(js.WriteString("").ExtractString() == "\"\"");

  REQUIRE(js.WriteString("hello").ExtractString() == "\"hello\"");
}

TEST_CASE("JsonBuilder overloaded write value") {
  JsonBuilder js;
  unsigned int ui = 1;
  REQUIRE(js.WriteValue(1).ExtractString() == "1");
  REQUIRE(js.WriteValue(ui).ExtractString() == "1");
  REQUIRE(js.WriteValue('1').ExtractString() == "49");

  REQUIRE(js.WriteValue(true).ExtractString() == "true");

  REQUIRE(js.WriteValue(1.1).ExtractString() == "1.1");

  REQUIRE(js.WriteValue("hello").ExtractString() == "\"hello\"");
  REQUIRE(js.WriteValue(std::string("hello")).ExtractString() == "\"hello\"");
}

TEST_CASE("JsonBuilder build object value") {
  JsonBuilder js;

  js.ObjectBegin().ObjectEnd();
  REQUIRE(js.ExtractString() == "{}");

  js.ObjectBegin()
    .WriteName("a").WriteInteger(1)
    .ObjectEnd();
  REQUIRE(js.ExtractString() == R"({"a":1})");

  js.ObjectBegin()
    .WriteName("a").WriteInteger(1)
    .WriteName("b").WriteBool(true)
    .ObjectEnd();
  REQUIRE(js.ExtractString() == R"({"a":1,"b":true})");

  js.ObjectBegin()
    .WriteName("o")
      .ObjectBegin()
      .WriteName("a").WriteInteger(1)
      .ObjectEnd()
    .WriteName("b").WriteBool(true)
    .ObjectEnd();
  REQUIRE(js.ExtractString() == R"({"o":{"a":1},"b":true})");
}

TEST_CASE("JsonBuilder build array value") {
  JsonBuilder js;

  js.ArrayBegin().ArrayEnd();
  REQUIRE(js.ExtractString() == "[]");

  js.ArrayBegin().WriteInteger(1).ArrayEnd();
  REQUIRE(js.ExtractString() == R"([1])");

  js.ArrayBegin()
    .WriteString("").WriteDouble(2.1)
    .ArrayEnd();
  REQUIRE(js.ExtractString() == R"(["",2.1])");

  js.ArrayBegin()
    .ArrayBegin()
    .ArrayEnd()
    .ArrayEnd();
  REQUIRE(js.ExtractString() == R"([[]])");
}

TEST_CASE("JsonBuilder build complex value") {
  JsonBuilder js;

  js.ObjectBegin()
    .WriteName("a").ArrayBegin()
      .ObjectBegin().WriteName("b").WriteInteger(1)
      .ObjectEnd()
    .ArrayEnd()
    .ObjectEnd();
  REQUIRE(js.ExtractString() == R"({"a":[{"b":1}]})");
}

struct Dumptable {
  Dumptable(int v) : value(v){}
  int value;
};

namespace cpplog {
  template<>
  struct dumper<Dumptable> {
    void operator()(JsonBuilder& jb, const Dumptable& value) {
      jb.WriteValue(value.value);
    }
  };
}

TEST_CASE("Dump to JsonBuilder") {
  JsonBuilder js;

  dump(js, 1);
  REQUIRE(js.ExtractString() == R"(1)");

  dump(js, 1.0);
  REQUIRE(js.ExtractString() == R"(1)");

  dump(js, 1.1);
  REQUIRE(js.ExtractString() == R"(1.1)");

  dump(js, true);
  REQUIRE(js.ExtractString() == R"(true)");

  dump(js, "hello");
  REQUIRE(js.ExtractString() == R"("hello")");

  dump(js, Dumptable(100));
  REQUIRE(js.ExtractString() == R"(100)");
}

