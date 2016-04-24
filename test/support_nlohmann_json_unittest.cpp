#include "catch.hpp"
#include <cpplog/json_builder.h>
#include <cpplog/support_nlohmann_json.h>

using namespace cpplog;
using namespace nlohmann;

struct TypeConvertible {
  int value;
};

json to_json(const TypeConvertible& value) {
  return value.value;
}

TEST_CASE("Support to log nlohmann json value") {
  json jv = 0;
  JsonBuilder jb;

  dump(jb, jv);
  REQUIRE(jb.ExtractString() == R"(0)");

  dump(jb, json(""));
  REQUIRE(jb.ExtractString() == R"("")");

  dump(jb, json(1000u));
  REQUIRE(jb.ExtractString() == R"(1000)");

  dump(jb, json(1.0));
  REQUIRE(jb.ExtractString() == R"(1)");

  // array
  jv = json{1, 2.1, "abc"};
  dump(jb, jv);
  REQUIRE(jb.ExtractString() == R"([1,2.1,"abc"])");

  // object
  jv = json{{"a", 1}, {"b", 2.1}, {"c", "abc"}};
  dump(jb, jv);
  REQUIRE(jb.ExtractString() == R"({"a":1,"b":2.1,"c":"abc"})");

  TypeConvertible t {-1};
  dump(jb, t);
  REQUIRE(jb.ExtractString() == R"(-1)");
}

