#pragma once
#include "cpplog/config.h"
#include <sstream>
#include <stack>

namespace cpplog {

class JsonBuilderException : public std::exception {
public:
  JsonBuilderException(const char* what)
    : what_(what) {
  }

  const char* what() const noexcept override {
    return what_;
  }

private:
  const char* what_;
};

class JsonBuilder {
public:
  JsonBuilder();

  void Reset();

  JsonBuilder(const JsonBuilder&) = delete;
  JsonBuilder& operator=(const JsonBuilder&) = delete;

  std::string ExtractString();

  JsonBuilder& WriteInteger(long long int value);
  JsonBuilder& WriteDouble(double value);
  JsonBuilder& WriteBool(bool value);
  JsonBuilder& WriteNull();
  JsonBuilder& WriteString(const std::string& value);

  JsonBuilder& WriteName(const std::string& value);

  template<typename T>
  JsonBuilder& WriteValue(T&& value);

  JsonBuilder& ObjectBegin();
  JsonBuilder& ObjectEnd();

  JsonBuilder& ArrayBegin();
  JsonBuilder& ArrayEnd();

private:
  enum StreamState {
    EXPECT_VALUE,
    EXPECT_FIRST_PROPERTY_NAME,
    EXPECT_MORE_PROPERTY_NAME,
    EXPECT_PROPERTY_VALUE,
    EXPECT_FIRST_ARRAY_VALUE,
    EXPECT_MORE_ARRAY_VALUE
  };

  std::stack<StreamState> state_stack_;

  void BeginValue();
  JsonBuilder& EndValue();

  std::ostringstream stream_;
};

namespace detail {
  template<typename T>
#if __cplusplus >= 201402L
  typename std::enable_if_t<std::is_integral<T>::value && !std::is_same<T, bool>::value, void>
#else
  typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, void>::type
#endif
  WriteValueHelp(JsonBuilder& stream, T value) {
    stream.WriteInteger(value);
  }

  template<typename T>
#if __cplusplus >= 201402L
  typename std::enable_if_t<std::is_same<T, bool>::value, void>
#else
  typename std::enable_if<std::is_same<T, bool>::value, void>::type
#endif
  WriteValueHelp(JsonBuilder& stream, T value) {
    stream.WriteBool(value);
  }

  template<typename T>
#if __cplusplus >= 201402L
  typename std::enable_if_t<std::is_floating_point<T>::value, void>
#else
  typename std::enable_if<std::is_floating_point<T>::value, void>::type
#endif
  WriteValueHelp(JsonBuilder& stream, T value) {
    stream.WriteDouble(value);
  }

  inline void WriteValueHelp(JsonBuilder& stream, const std::string&& value) {
    stream.WriteString(value);
  }

  inline void WriteValueHelp(JsonBuilder& stream, const char* value) {
    stream.WriteString(value);
  }

  template<typename T>
  struct is_basic_value {
    static constexpr bool value = std::is_integral<T>::value
                                  || std::is_floating_point<T>::value
                                  || std::is_same<T, std::string>::value
                                  || std::is_constructible<std::string, T>::value;
  };
}

template<typename T>
JsonBuilder& JsonBuilder::WriteValue(T&& value) {
  detail::WriteValueHelp(*this, std::forward<T>(value));
  return *this;
}

template<typename T>
struct dumper {
  void operator()(JsonBuilder& jb, T value) {
    // use to_json function
    dump(jb, to_json(value));
  }
};

template<typename T>
#if __cplusplus >= 201402L
typename std::enable_if_t<detail::is_basic_value<T>::value>
#else
typename std::enable_if<detail::is_basic_value<T>::value>::type
#endif
  dump(JsonBuilder& json_builder, T&& value) {
  json_builder.WriteValue(std::forward<T>(value));
}

template<typename T>
#if __cplusplus >= 201402L
  typename std::enable_if_t<!detail::is_basic_value<T>::value>
#else
  typename std::enable_if<!detail::is_basic_value<T>::value>::type
#endif
  dump(JsonBuilder& json_builder, T&& value) {
  dumper<typename std::decay<T>::type>{}(json_builder, std::forward<T>(value));
}

} // namespace cpplog

#include "cpplog/json_builder_inl.h"

