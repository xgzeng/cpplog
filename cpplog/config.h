#pragma once

#define CPPLOG_INLINE inline

#include <string>
#include <stdexcept>

namespace cpplog {

class string_view {
public:
  static const size_t npos = (size_t)(-1);

  string_view()
  : data_(nullptr), data_size_(0) {
  }

  string_view(const std::string& s)
  : data_(s.data()), data_size_(s.size()){
  }

  string_view(const char* s, size_t count)
  : data_(s), data_size_(count){
  }

  string_view(const char* s)
  : data_(s), data_size_(strlen(s)) {
  }

  operator std::string () const {
    return std::string(data_, data_size_);
  }

  bool operator == (const string_view& other) const {
    if (data_size_ != other.data_size_) return false;
    for (size_t i = 0; i < data_size_; ++i) {
      if (data_[i] != other.data_[i]) return false;
    }
    return true;
  }

  const char* data() const {
    return data_;
  }

  size_t size() const {
    return data_size_;
  }

  size_t length() const {
    return data_size_;
  }

  const char* begin() const {
    return data_;
  }

  const char* end() const {
    return data_ + data_size_;
  }

  size_t find_first_of(char c, size_t pos = 0) const {
    pos = std::min(pos, data_size_);
    for (auto p = data_ + pos; p != (data_ + data_size_); ++p) {
      if (*p == c) {
        return p - data_;
      }
    }
    return npos;
  }

  string_view substr(size_t pos = 0, size_t count = npos) {
    if (pos > data_size_) throw std::out_of_range("string_view.substr() out of range");
    return string_view(data_ + pos, std::min(count, size() - pos));
  }

private:
  const char* data_;
  size_t data_size_;
};

} // namespace cpplog
