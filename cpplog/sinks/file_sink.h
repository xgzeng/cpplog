#pragma once
#include "cpplog/config.h"

namespace cpplog {

class FileSink : public LogSink {
public:
  FileSink(string_view base_file_name);

  void SubmitRecord(const LogRecord&) override;

private:
  std::string base_file_name_;
  std::mutex mutex_;

  bool CreateFile(string_view file_name);
};

} // namespace cpplog

inline FileSink::FileSink(string_view base_name)
: base_file_name_(base_name) {
}

inline void FileSink::SubmitRecord(const LogRecord& r) {
  std::lock_guard<std::mutex> l(mutex_);
}
