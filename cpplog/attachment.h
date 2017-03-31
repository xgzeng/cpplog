#pragma once

#include "cpplog/config.h"
#include "cpplog/utils.h"

namespace cpplog {

class Attachment {
public:
  Attachment();

  bool empty() const {
    return map_size_ == 0;
  }

  size_t size() const {
    return map_size_;
  }

  // add integer property
  template<typename T,
          typename std::enable_if<helper::is_integer<T>::value, int>::type = 0>
  void add(string_view name, T value);

  // add string property
  void add(string_view name, string_view value);

  /*! add bool property
      @note use enable_if trick to disable convert 'const char*' to bool type
   */
  template<typename T,
           typename std::enable_if<std::is_same<T, bool>::value, int>::type = 0>
  void add(string_view name, T value);

  void add(string_view name, std::nullptr_t);

  void add(string_view name, float value);

  void add(string_view name, double value);

  string_view msgpack_data() const;

  template<typename V>
  void visit(V&& v) const;

private:
  size_t map_size_ = 0;
  std::string msgpack_map_data_;
};

} // namespace cpplog

#include "cpplog/attachment_inl.h"
