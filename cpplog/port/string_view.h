#pragma once

#include <string>  // strlen
#include <cstring>

#ifdef __has_include                           // Check if __has_include is present
  #if __has_include(<string_view>)
    #include <string_view>
  #elif __has_include(<experimental/string_view>)
    #include <experimental/string_view>
  #endif
#else
  #error "no __has_include"
#endif

namespace cpplog {

#ifdef __cpp_lib_string_view
  using std::string_view;
#elif defined(__cpp_lib_experimental_string_view)
  using std::experimental::string_view;
#else
/// trival string_view implement
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

  bool empty() const {
    return data_size_ == 0;
  }

  const char* begin() const {
    return data_;
  }

  const char* end() const {
    return data_ + data_size_;
  }

  size_t find_first_of(char c, size_t pos = 0) const {
    pos = pos < data_size_ ? pos : data_size_;
    for (auto p = data_ + pos; p != (data_ + data_size_); ++p) {
      if (*p == c) {
        return p - data_;
      }
    }
    return npos;
  }

  string_view substr(size_t pos = 0, size_t count = npos) {
    if (pos > data_size_) throw std::out_of_range("string_view.substr() out of range");
    size_t rcount = count < size() - pos ? count : size() - pos;
    return string_view(data_ + pos, rcount);
  }

private:
  const char* data_;
  size_t data_size_;
};
#endif
  
} // namespace cpplog
