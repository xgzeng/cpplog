#include "cpplog/logging.h"
#include "cpplog/sinks/console_sink.h"
#include "cpplog/hex.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "fakeit.hpp"

using namespace cpplog;
using namespace fakeit;

struct TestSink : public LogSink {
  void Submit(const LogRecord& record) override {
    records.push_back(record);
  }

  std::string last_message() const {
    if (records.empty()) {
      return "";
    }
    return records.back().message();
  }

  std::vector<LogRecord> records;
};

struct ScopedTestSink : TestSink {
  ScopedTestSink() {
    AddLogSink(this);
  }

  ~ScopedTestSink() {
    RemoveLogSink(this);
  }
};

struct StreamableObject {
  int value;
};

std::ostream& operator << (std::ostream& os, StreamableObject o) {
  return os << o.value;
}

TEST_CASE("LogCapture stream interface") {
  TestSink log_result;

  LOG_TO(log_result, INFO, "");
  REQUIRE(log_result.last_message() == "");

  LOG_TO(log_result, ERROR, "") << 1;
  REQUIRE(log_result.last_message() == "1");

  LOG_TO(log_result, WARNING, "") << 1 << "hello";
  REQUIRE(log_result.last_message() == "1hello");

  LOG_TO(log_result, FATAL, "") << 1 << "hello" << StreamableObject{2};
  REQUIRE(log_result.last_message() == "1hello2");
}

TEST_CASE("LogCapture format interface") {
  TestSink log_result;

  LOG_TO(log_result, INFO, "");
  REQUIRE(log_result.last_message() == "");

  LOG_TO(log_result, INFO, "{}", 1);

  REQUIRE(log_result.last_message() == "1");

  LOG_TO(log_result, INFO,
        "{}{}{}", 1, "hello", StreamableObject{2});

  REQUIRE(log_result.last_message() == "1hello2");

  LOG_TO(log_result, INFO,
        "{}{}", 1, "hello") << StreamableObject{2};

  REQUIRE(log_result.last_message() == "1hello2");
}

TEST_CASE("cpplog hexify") {
  TestSink log_result;

  std::string s{ '\x00', '\x01' };
  LOG_TO(log_result, INFO, "{}", hexify(s));
  REQUIRE(log_result.last_message() == "00 01");

  char char_array[] = {'\x01', '\x00', '\xFF'};
  LOG_TO(log_result, INFO, "{}", hexify(char_array));
  REQUIRE(log_result.last_message() == "01 00 FF");

  std::vector<uint8_t> vec_uint8{ 0, 16, 0xFF};
  LOG_TO(log_result, INFO,  "{}", hexify(vec_uint8));
  REQUIRE(log_result.last_message() == "00 10 FF");
}

TEST_CASE("logging macros") {
  SetLogToConsole(false);
  ScopedTestSink scoped_sink;
  SECTION("Use Macro To Log") {
    LOG(INFO, "hello, macros");
    REQUIRE(scoped_sink.last_message() == "hello, macros");
  }

  SECTION("Log With Level filtering") {
    LogDispatcher::instance().EnableLevelAbove(LogLevel::Error);
    LOG(INFO, "hello, info level");
    REQUIRE(scoped_sink.last_message() == "");
    LogDispatcher::instance().EnableLevelAbove(LogLevel::Information);
  }

  SECTION("LOG_IF") {
    WHEN("condition is true") {
      LOG_IF(INFO, true, "TEST_LOG_IF(true)");
      REQUIRE(scoped_sink.last_message() == "TEST_LOG_IF(true)");
    }

    WHEN("condition is false") {
      LOG_IF(ERROR, false, "TEST_LOG_IF(false)");
      REQUIRE(scoped_sink.last_message() == "");
    }
  }

  SECTION("LOG_TO_IF") {
    LOG_TO_IF(scoped_sink, ERROR, true, "if_true");
    REQUIRE(scoped_sink.last_message() == "if_true");

    LOG_TO_IF(scoped_sink, ERROR, false, "if_false");
    REQUIRE(scoped_sink.last_message() == "if_true");
  }
}

TEST_CASE("LOG_EVERY_N") {

  for (int i = 0; i <= 100; ++i) {
    ScopedTestSink scoped_sink;
    LOG_EVERY_N(INFO, 10, "LOG_EVERY_{}", i);
    if (i % 10 == 0) {
      REQUIRE(scoped_sink.last_message() == fmt::format("LOG_EVERY_{}", i));
    } else {
      REQUIRE(scoped_sink.last_message() == "");
    }
  }
}
