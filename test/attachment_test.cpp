#include "cpplog/attachment.h"

#include "catch.hpp"
#include "fakeit.hpp"

#ifdef HAVE_NLOHMANN_JSON
#include "json.hpp"
#endif

using namespace cpplog;
using namespace fakeit;

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

TEST_CASE("Attatchment class test") {
  Mock<TestAttachmentVisitor> mock_visitor;

  Attachment attachment;

  SECTION("empty attachment") {
    REQUIRE(attachment.empty());
    attachment.visit(mock_visitor.get()); // nothing is called
  }

  SECTION("Int property") {
    Fake(Method(mock_visitor, on_int));
    attachment.add("abc", 1);
    attachment.visit(mock_visitor.get());
    Verify(Method(mock_visitor, on_int).Using("abc", 1)).Once();
  }

  SECTION("Multiple Properties") {
    Fake(Method(mock_visitor, on_bool));
    Fake(Method(mock_visitor, on_double));
    Fake(Method(mock_visitor, on_string));

    attachment.add("b", false);
    attachment.add("f", 1.1);
    attachment.add("s", "string");

    attachment.visit(mock_visitor.get());

    Verify(Method(mock_visitor, on_bool).Using("b", false)).Once();
    Verify(Method(mock_visitor, on_double).Using("f", 1.1)).Once();
    Verify(Method(mock_visitor, on_string).Using("s", "string")).Once();
  }
}

#ifdef HAVE_NLOHMANN_JSON
TEST_CASE("use nlohmann::json to decode msgpack data") {
  Attachment attachment;
  attachment.add("field0", 1);
  attachment.add("field1", true);
  attachment.add("field2", 1.1);
  attachment.add("field3", "abc");
  attachment.add("field4", nullptr);

  std::string msgpack_data;

  msgpack_pack_map(StringBufferWriter(msgpack_data),
                   attachment.size());

  auto map_body = attachment.msgpack_data();
  msgpack_data.append(map_body.begin(), map_body.end());

  printf("%s\n", hexify(msgpack_data).c_str());

  auto test_value = nlohmann::json::from_msgpack({msgpack_data.begin(), msgpack_data.end()});

  nlohmann::json expected_value = {
      {"field0", 1},
      {"field1", true},
      {"field2", 1.1},
      {"field3", "abc"},
      {"field4", nullptr},
  };

  REQUIRE(test_value == expected_value);
}

#endif

