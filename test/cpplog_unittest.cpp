#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "cpplog/logging.h"
#include "cpplog/console_sink.h"
#include "cpplog/hex.h"

using namespace cpplog;

#define LOG_CAPTURE(sink) LogCapture(sink, LogLevel::info, __FILE__, __LINE__, __func__)

struct TestSink : public LogSink, public LogRecord {
  void SubmitRecord(LogRecord& record) override {
    (LogRecord&)(*this) = record;
  }

  void reset() {
      (LogRecord&)(*this) = LogRecord();
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

TEST_CASE("cpplog hexify") {
  std::string s{ '\x00', '\x01' };
  LOG_CAPTURE(log_result).message("{}", hexify(s));
  REQUIRE(log_result.message() == "00 01");

  char char_array[] = {'\x01', '\x00', '\xFF'};
  LOG_CAPTURE(log_result).message("{}", hexify(char_array));
  REQUIRE(log_result.message() == "01 00 FF");

  std::vector<uint8_t> vec_uint8{ 0, 16, 0xFF};
  LOG_CAPTURE(log_result).message("{}", hexify(vec_uint8));
  REQUIRE(log_result.message() == "00 10 FF");
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

    log_result.reset();
    REQUIRE(log_result.message() == "");

    SECTION("Use Macro To Log") {
        LOG(INFO, "hello, macros");
        REQUIRE(log_result.message() == "hello, macros");
    }

    SECTION("Log With Level filtering") {
        LogDispatcher::instance().EnableLevelAbove(LogLevel::error);
        LOG(INFO, "hello, info level");
        REQUIRE(log_result.message() == "");
   }
}
