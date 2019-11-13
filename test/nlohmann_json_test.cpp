#include "cpplog/logging.h"
#include "cpplog/nlohmann_json.h"
#include "cpplog/hex.h"

#include "catch.hpp"

TEST_CASE("nlohmann::json format") {
  std::string s = fmt::format("{}", nlohmann::json::object());
  REQUIRE(s == "{}");
}
