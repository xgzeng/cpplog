#pragma once
#include "cpplog/config.h"
#include <cstdio>
#include <atomic>
#include <vector>

namespace cpplog {

class FileSink : public LogSink {
public:
  FileSink();
  
  FileSink(string_view base_name);
  
  const std::string base_name() const {
    return base_name_;
  }
  
  void set_base_name(string_view name) {
    base_name_ = name;
  }

  void set_max_file_length(unsigned int max_length) {
    max_file_length_ = max_length;
  }
  
  void set_log_dirs(const std::vector<std::string>& dirs) {
    log_dirs_ = dirs;
  }

  const std::string& current_logfile_path() const;

  void CloseLogFile();

  // LogSink interface
  void Submit(const LogRecord&) override;

private:
  static const int kRolloverAttemptFrequency = 0x20;

  std::vector<std::string> log_dirs_;

  std::string base_name_;

  mutable std::mutex mutex_;

  // used to avoid try create file too often
  int rollover_attempt_ = kRolloverAttemptFrequency;

  std::unique_ptr<std::FILE, int (*)(std::FILE*)> file_;
  bool CreateLogFile(const std::string& file_name);

  std::atomic<unsigned int> max_file_length_{ 20 * 1024 * 1024 }; // 20MB

  std::string current_logfile_path_;

  unsigned int file_length_{ 0 };
  int bytes_since_flush_{ 0 };
  void FlushUnlocked();
  void CloseFileUnlocked();
};

// helpers to simpilfy file sink initialization
struct FileSinkBaseName {
  FileSinkBaseName(string_view base_name)
    :base_name_(base_name) {
  }
  
  void operator()(FileSink& sink) {
    sink.set_base_name(base_name_);
  }
  
private:
  std::string base_name_;
};

struct FileSinkDirs {
  
  FileSinkDirs(std::initializer_list<std::string> args)
  : dirs_{args} {
  }
  
  // FileSinkDirs(std::initializer_list<T> args)
  // : dirs_{args} {
  // }
  
  void operator()(FileSink& sink) {
    sink.set_log_dirs(dirs_);
  }
  
private:
  std::vector<std::string> dirs_;
};

} // namespace cpplog

#include "cpplog/sinks/file_sink_inl.h"
