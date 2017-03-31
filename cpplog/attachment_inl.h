#include "cpplog/msgpack_pack.h"
#include "cpplog/msgpack_unpack.h"

namespace cpplog {

CPPLOG_INLINE Attachment::Attachment() {
}

CPPLOG_INLINE void Attachment::add(string_view name, string_view value) {
  StringBufferWriter writer(msgpack_map_data_);
  msgpack_pack_str(writer, name);
  msgpack_pack_str(writer, value);
  ++map_size_;
}

template<typename T, typename std::enable_if<helper::is_integer<T>::value, int>::type>
CPPLOG_INLINE void Attachment::add(string_view name, T value) {
  StringBufferWriter writer(msgpack_map_data_);
  msgpack_pack_str(writer, name);
  msgpack_pack_int(writer, value);
  ++map_size_;
}

template<typename T,
         typename std::enable_if<std::is_same<T, bool>::value, int>::type>
CPPLOG_INLINE void Attachment::add(string_view name, T value) {
  StringBufferWriter writer(msgpack_map_data_);
  msgpack_pack_str(writer, name);
  msgpack_pack_boolean(writer, value);
  ++map_size_;
}

CPPLOG_INLINE void Attachment::add(string_view name, std::nullptr_t) {
  StringBufferWriter writer(msgpack_map_data_);
  msgpack_pack_str(writer, name);
  msgpack_pack_nil(writer);
  ++map_size_;
}

CPPLOG_INLINE void Attachment::add(string_view name, float value) {
  StringBufferWriter writer(msgpack_map_data_);
  msgpack_pack_str(writer, name);
  msgpack_pack_float(writer, value);
  ++map_size_;
}

CPPLOG_INLINE void Attachment::add(string_view name, double value) {
  StringBufferWriter writer(msgpack_map_data_);
  msgpack_pack_str(writer, name);
  msgpack_pack_double(writer, value);
  ++map_size_;
}

namespace helper {

template<typename V>
struct AttachmentMsgpackVisitor : msgpack_visitor {
  V& visitor;
  bool next_is_name_field;
  string_view name_field;

  AttachmentMsgpackVisitor(V& v)
  : visitor(v), next_is_name_field(true) {
  }

  void on_int(int value) override {
    assert(!next_is_name_field);
    visitor(name_field, value);
    next_is_name_field = true;
  }

  void on_uint(unsigned int value) override {
    assert(!next_is_name_field);
    visitor(name_field, value);
    next_is_name_field = true;
  }

  void on_longlong (long long value) override {
    assert(!next_is_name_field);
    visitor(name_field, value);
    next_is_name_field = true;
  }

  void on_ulonglong(unsigned long long value) override {
    assert(!next_is_name_field);
    visitor(name_field, value);
    next_is_name_field = true;
  }

  void on_float(float value) override {
    assert(!next_is_name_field);
    visitor(name_field, value);
    next_is_name_field = true;
  }

  void on_double(double value)override  {
    assert(!next_is_name_field);
    visitor(name_field, value);
    next_is_name_field = true;
  }

  void on_string(string_view s) override {
    if (next_is_name_field) {
      name_field = s;
      next_is_name_field = false;
    } else {
      visitor(name_field, s);
      next_is_name_field = true;
    }
  }

  void on_nil() override {
    assert(!next_is_name_field);
    visitor(name_field, nullptr);
    next_is_name_field = true;
  }

  void on_boolean(bool value) override {
    assert(!next_is_name_field);
    visitor(name_field, value);
    next_is_name_field = true;
  }

  void on_array(uint32_t array_size) override {
    // should be unreachable now
    assert(false);
  }

  void on_map(uint32_t map_size) override {
    // should be unreachable now
    assert(false);
  }
};

} // namespace helper

template<typename V>
CPPLOG_INLINE void Attachment::visit(V&& visitor) const {
  helper::AttachmentMsgpackVisitor<V> proxy_visitor(visitor);
  msgpack_unpack(msgpack_map_data_, proxy_visitor);
}

CPPLOG_INLINE string_view Attachment::msgpack_data() const {
  return msgpack_map_data_;
}

} // namespace cpplog

