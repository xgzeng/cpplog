#pragma once

#define CPPLOG_INLINE inline

#include <string>

namespace cpplog {

class string_view {
public:
  string_view()
  : data_(nullptr), count_(0) {
  }

  string_view(const std::string& s)
  : data_(s.data()), count_(s.size()){
  }

  string_view(const char* s, size_t count)
  : data_(s), count_(count){
  }

  string_view(const char* s)
  : data_(s), count_(strlen(s)) {
  }

  operator std::string () const {
    return std::string(data_, count_);
  }

  bool operator == (const string_view& other) const {
    if (count_ != other.count_) return false;
    for (size_t i = 0; i < count_; ++i) {
      if (data_[i] != other.data_[i]) return false;
    }
    return true;
  }

  const char* data() const {
    return data_;
  }

  size_t size() const {
    return count_;
  }

  size_t length() const {
    return count_;
  }

  const char* begin() const {
    return data_;
  }

  const char* end() const {
    return data_ + count_;
  }

private:
  const char* data_;
  size_t count_;
};

} // namespace cpplog
