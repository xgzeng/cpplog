#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "cpplog/logging.h"
#include "cpplog/sinks/console_sink.h"
#include "cpplog/hex.h"
#include "fakeit.hpp"

using namespace cpplog;
using namespace fakeit;

struct TestSink : public LogSink, public LogRecord {
  void Submit(const LogRecord& record) override {
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
  LOG_TO(log_result, INFO, "");
  REQUIRE(log_result.message() == "");

  LOG_TO(log_result, ERROR, "") << 1;
  REQUIRE(log_result.message() == "1");

  LOG_TO(log_result, WARNING, "") << 1 << "hello";
  REQUIRE(log_result.message() == "1hello");

  LOG_TO(log_result, FATAL, "") << 1 << "hello" << StreamableObject{2};
  REQUIRE(log_result.message() == "1hello2");
}

TEST_CASE("LogCapture format interface") {
  LOG_TO(log_result, INFO, "");
  REQUIRE(log_result.message() == "");

  LOG_TO(log_result, INFO, "{}", 1);

  REQUIRE(log_result.message() == "1");

  LOG_TO(log_result, INFO,
        "{}{}{}", 1, "hello", StreamableObject{2});

  REQUIRE(log_result.message() == "1hello2");

  LOG_TO(log_result, INFO,
        "{}{}", 1, "hello") << StreamableObject{2};

  REQUIRE(log_result.message() == "1hello2");
}

TEST_CASE("cpplog hexify") {
  std::string s{ '\x00', '\x01' };
  LOG_TO(log_result, INFO, "{}", hexify(s));
  REQUIRE(log_result.message() == "00 01");

  char char_array[] = {'\x01', '\x00', '\xFF'};
  LOG_TO(log_result, INFO, "{}", hexify(char_array));
  REQUIRE(log_result.message() == "01 00 FF");

  std::vector<uint8_t> vec_uint8{ 0, 16, 0xFF};
  LOG_TO(log_result, INFO,  "{}", hexify(vec_uint8));
  REQUIRE(log_result.message() == "00 10 FF");
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
    LogDispatcher::instance().EnableLevelAbove(LogLevel::Error);
    LOG(INFO, "hello, info level");
    REQUIRE(log_result.message() == "");
    LogDispatcher::instance().EnableLevelAbove(LogLevel::Information);
  }

  SECTION("LOG_IF") {
    WHEN("condition is true") {
      LOG_IF(INFO, true, "TEST_LOG_IF(true)");
      REQUIRE(log_result.message() == "TEST_LOG_IF(true)");
    }

    WHEN("condition is false") {
      LOG_IF(ERROR, false, "TEST_LOG_IF(false)");
      REQUIRE(log_result.message() == "");
    }
  }

  SECTION("LOG_TO_IF") {
    LOG_TO_IF(log_result, ERROR, true, "if_true");
    REQUIRE(log_result.message() == "if_true");

    LOG_TO_IF(log_result, ERROR, false, "if_false");
    REQUIRE(log_result.message() == "if_true");
  }
}

TEST_CASE("LOG_EVERY_N") {
  AddLogSink(&log_result);
  for (int i = 0; i <= 100; ++i) {
    log_result.reset();
    LOG_EVERY_N(INFO, 10, "LOG_EVERY_{}", i);
    if (i % 10 == 0) {
      REQUIRE(log_result.message() == fmt::format("LOG_EVERY_{}", i));
    } else {
      REQUIRE(log_result.message() == "");
    }
  }
}
