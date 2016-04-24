#include <cpplog/config.h>
#include <cassert>

#define CHECK_STREAM_IS_OPEND()        \
  do {                               \
    if (state_stack_.empty())         \
      throw JsonBuilderException("json stream is closed"); \
  } while(0)

namespace cpplog {

CPPLOG_INLINE JsonBuilder::JsonBuilder() {
  Reset();
}

CPPLOG_INLINE void JsonBuilder::Reset() {
  stream_.str("");
  state_stack_.push(EXPECT_VALUE);
}

CPPLOG_INLINE std::string JsonBuilder::ExtractString() {
  if (!state_stack_.empty()) {
    throw JsonBuilderException("json stream is not ended properly");
  }
  auto ret = stream_.str();
  Reset();
  return ret;
}

CPPLOG_INLINE void JsonBuilder::BeginValue() {
  CHECK_STREAM_IS_OPEND();

  switch (state_stack_.top()) {
  case EXPECT_VALUE:
    break;
  case EXPECT_FIRST_ARRAY_VALUE:
    break;
  case EXPECT_MORE_ARRAY_VALUE:
    stream_ << ",";
    break;
  case EXPECT_FIRST_PROPERTY_NAME:
    break;
  case EXPECT_MORE_PROPERTY_NAME:
    stream_ << ",";
    break;
  case EXPECT_PROPERTY_VALUE:
    break;
  default:
    throw JsonBuilderException("invalid json stream state");
  }
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::EndValue() {
  CHECK_STREAM_IS_OPEND();

  switch (state_stack_.top()) {
  case EXPECT_VALUE:
    state_stack_.pop();
    break;
  case EXPECT_FIRST_PROPERTY_NAME:
    throw JsonBuilderException("invalid json stream state");
  case EXPECT_PROPERTY_VALUE:
    state_stack_.top() = EXPECT_MORE_PROPERTY_NAME;
    break;
  case EXPECT_MORE_PROPERTY_NAME:
    state_stack_.top() = EXPECT_PROPERTY_VALUE;
    break;
  case EXPECT_FIRST_ARRAY_VALUE:
    state_stack_.top() = EXPECT_MORE_ARRAY_VALUE;
    break;
  case EXPECT_MORE_ARRAY_VALUE:
    break;
   defaut:
    assert(false);
  }

  return *this;
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::WriteInteger(long long int value) {
  BeginValue();
  stream_ << value;
  return EndValue();
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::WriteDouble(double value) {
  BeginValue();
  stream_ << value;
  return EndValue();
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::WriteBool(bool value) {
  BeginValue();
  stream_ << (value ? "true" : "false");
  return EndValue();
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::WriteNull() {
  BeginValue();
  stream_ << "null";
  return EndValue();
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::WriteString(const std::string& value) {
  BeginValue();
  stream_ << '"' << value << '"';
  return EndValue();
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::WriteName(const std::string& value) {
  CHECK_STREAM_IS_OPEND();

  switch (state_stack_.top()) {
  case EXPECT_MORE_PROPERTY_NAME:
    stream_ << ",";   // continue to next case is intented
  case EXPECT_FIRST_PROPERTY_NAME:
    // TODO: escape string value
    stream_ << '"' << value << "\":";
    state_stack_.top() = EXPECT_PROPERTY_VALUE;
    break;
  default:
    throw JsonBuilderException("invalid json stream state");
  }

  return *this;
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::ObjectBegin() {
  BeginValue();

  stream_ << "{";
  state_stack_.push(EXPECT_FIRST_PROPERTY_NAME);
  return *this;
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::ObjectEnd() {
  CHECK_STREAM_IS_OPEND();

  switch (state_stack_.top()) {
  case EXPECT_FIRST_PROPERTY_NAME:
  case EXPECT_MORE_PROPERTY_NAME:
    stream_ << "}";
    break;
  default:
    throw JsonBuilderException("invalid json stream state");
  }

  state_stack_.pop();
  return EndValue();
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::ArrayBegin() {
  BeginValue();

  stream_ << "[";
  state_stack_.push(EXPECT_FIRST_ARRAY_VALUE);

  return *this;
}

CPPLOG_INLINE JsonBuilder& JsonBuilder::ArrayEnd() {
  CHECK_STREAM_IS_OPEND();

  switch(state_stack_.top()) {
  case EXPECT_FIRST_ARRAY_VALUE:
  case EXPECT_MORE_ARRAY_VALUE:
    stream_ << "]";
    break;
  default:
    throw JsonBuilderException("invalid json stream state");
  }
  state_stack_.pop();

  return EndValue();
}

}

