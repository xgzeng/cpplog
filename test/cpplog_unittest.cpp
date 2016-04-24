#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cpplog/logging.h>

using namespace cpplog;

#define LOG_CAPTURE(sink) LogCapture(sink, LogLevel::info, __FILE__, __LINE__, __func__)

struct TestSink : public LogSink, public LogRecord {
  void SubmitRecord(LogRecord& record) override {
    (LogRecord&)(*this) = record;
  }
} log_result;

struct StreamableObject {
  int value;
};

std::ostream& operator << (std::ostream& os, StreamableObject o) {
  return os << o.value;
}

TEST_CASE("LogCapture stream interface") {
  LOG_CAPTURE(log_result);
  REQUIRE(log_result.message() == "");

  LOG_CAPTURE(log_result) << 1;
  REQUIRE(log_result.message() == "1");

  LOG_CAPTURE(log_result) << 1 << "hello";
  REQUIRE(log_result.message() == "1hello");

  LOG_CAPTURE(log_result) << 1 << "hello" << StreamableObject{2};
  REQUIRE(log_result.message() == "1hello2");
}

TEST_CASE("LogCapture format interface") {
  LOG_CAPTURE(log_result).message("");
  REQUIRE(log_result.message() == "");

  LOG_CAPTURE(log_result).message("{}", 1);
  REQUIRE(log_result.message() == "1");

  LOG_CAPTURE(log_result).message("{}{}{}", 1, "hello", StreamableObject{2});
  REQUIRE(log_result.message() == "1hello2");

  LOG_CAPTURE(log_result).message("{}{}", 1, "hello") << StreamableObject{2};
  REQUIRE(log_result.message() == "1hello2");
}

TEST_CASE("LogCapture json properties") {
  LOG_CAPTURE(log_result);
  REQUIRE(log_result.message() == "");
  REQUIRE(log_result.fields().empty());

  LOG_CAPTURE(log_result)("abc", 1);
  REQUIRE(log_result.field("abc") == "1");

  LOG_CAPTURE(log_result)("b", false)("f", 1.1)("s", "string");
  REQUIRE(log_result.field("b") == "false");
  REQUIRE(log_result.field("f") == "1.1");
  REQUIRE(log_result.field("s") == "\"string\"");
}

TEST_CASE("logging macros") {
  SetLogToConsole(false);
  AddLogSink(&log_result);
  LOG(INFO, "hello, macros");
  REQUIRE(log_result.message() == "hello, macros");
}

