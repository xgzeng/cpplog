#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "cpplog/logging.h"
#include "cpplog/console_sink.h"
#include "cpplog/hex.h"

#include "fakeit.hpp"

using namespace cpplog;
using namespace fakeit;

#define LOG_CAPTURE(sink) LogCapture(sink, LogLevel::Information, __FILE__, __LINE__, __func__)

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

#if 0
struct TestAttachmentVisitor {
  virtual void on_int(cpplog::string_view name, int value) = 0;
  virtual void on_uint(cpplog::string_view name, unsigned int value) = 0;
  virtual void on_int64(cpplog::string_view name, int64_t value) = 0;
  virtual void on_uint64(cpplog::string_view name, uint64_t value) = 0;
  virtual void on_double(cpplog::string_view name, double value) = 0;
  virtual void on_bool(cpplog::string_view name, bool value) = 0;
  virtual void on_nil(cpplog::string_view name) = 0;
  virtual void on_string(cpplog::string_view name, string_view value) = 0;

  void operator()(cpplog::string_view name, int value) {
    on_int(name, value);
  }

  void operator()(cpplog::string_view name, unsigned int value) {
    on_uint(name, value);
  }

  void operator()(cpplog::string_view name, int64_t value) {
    on_int64(name, value);
  }

  void operator()(cpplog::string_view name, uint64_t value) {
    on_uint64(name, value);
  }

  void operator()(cpplog::string_view name, double value) {
    on_double(name, value);
  }

  void operator()(cpplog::string_view name, bool value) {
    on_bool(name, value);
  }

  void operator()(cpplog::string_view name, std::nullptr_t) {
    on_nil(name);
  }

  void operator()(cpplog::string_view name, cpplog::string_view value) {
    on_string(name, value);
  }
};

TEST_CASE("LogCapture json properties") {
  Mock<TestAttachmentVisitor> mock_visitor;

  SECTION("empty attachment") {
    LOG_CAPTURE(log_result);
    REQUIRE(log_result.message() == "");
    REQUIRE(log_result.attachment().empty());
    log_result.attachment().visit(mock_visitor.get());
  }

  SECTION("int property") {
    Fake(Method(mock_visitor, on_int));

    LOG_CAPTURE(log_result)("abc", 1);
    log_result.attachment().visit(mock_visitor.get());

    Verify(Method(mock_visitor, on_int).Using("abc", 1)).Once();
  }

  SECTION("Multiple Properties") {
    Fake(Method(mock_visitor, on_bool));
    Fake(Method(mock_visitor, on_double));
    Fake(Method(mock_visitor, on_string));

    LOG_CAPTURE(log_result)("b", false)("f", 1.1)("s", "string");
    log_result.attachment().visit(mock_visitor.get());

    Verify(Method(mock_visitor, on_bool).Using("b", false)).Once();
    Verify(Method(mock_visitor, on_double).Using("f", 1.1)).Once();
    Verify(Method(mock_visitor, on_string).Using("s", "string")).Once();
  }
}
#endif

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
   }
}
