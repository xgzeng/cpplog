#ifndef _WIN32
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

namespace cpplog {

CPPLOG_INLINE std::string GetExecutableBaseName() {
#if _WIN32
  const char PATH_SLASH_CHAR = '\\';
  TCHAR file_path[MAX_PATH + 1];
  DWORD file_name_len = GetModuleFileName(NULL, file_path, MAX_PATH + 1);
  if (file_name_len == 0) {
    return "unknown";
  }
#else
  const char PATH_SLASH_CHAR = '/';
  char file_path[1024];
  int len = readlink("/proc/self/exe", file_path, sizeof(file_path) - 1);
  if(len ==-1) {
    perror("GetExecutableBaseName");
    return "unknown";
  }
  file_path[len] = 0;
#endif

  std::string path = file_path;
  // find last '/'
  auto slash_pos = path.rfind(PATH_SLASH_CHAR);
  // find last '.'
  auto dot_pos = path.rfind('.');

  auto& npos = std::string::npos;
  auto begin_pos = slash_pos == npos ? 0 : slash_pos + 1;
  auto count = dot_pos == npos ? npos : dot_pos - begin_pos;
  return path.substr(begin_pos, count);
}

CPPLOG_INLINE FileSink::FileSink()
: FileSink(string_view{}) {
}

CPPLOG_INLINE FileSink::FileSink(string_view base_name)
: base_name_(base_name),
  file_{nullptr, fclose} {
  log_dirs_ = { "." };
  if (base_name_.empty()) {
    base_name_ = GetExecutableBaseName();
  }
}

CPPLOG_INLINE const std::string& FileSink::current_logfile_path() const {
  std::lock_guard<std::mutex> l(mutex_);
  return current_logfile_path_;
}

CPPLOG_INLINE void FileSink::Submit(const LogRecord& r) {
  std::lock_guard<std::mutex> l(mutex_);

  if (!file_) {
    if (++rollover_attempt_ < kRolloverAttemptFrequency) {
      return;
    }
    rollover_attempt_ = 0;

    // generate log file name
    time_t timestamp;
    time(&timestamp);
    struct ::tm tm_time;
#ifdef _WIN32
    localtime_s(&tm_time, &timestamp);
#else
    localtime_r(&timestamp, &tm_time);
#endif

#ifdef _WIN32
    auto pid = GetCurrentProcessId();
#else
    int pid = getpid();
#endif

    auto filename = fmt::format("{}{}.{}{:0>2}{:0>2}-{:0>2}{:0>2}{:0>2}.{}.log",
        base_name_, suffix_name_,
        tm_time.tm_year + 1900, tm_time.tm_mon, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
        pid);

    // try to create log file in various directory
    bool create_file_success = false;
    for (const auto& dir : log_dirs_) {
#if _WIN32
      auto file_path = dir + "\\" + filename;
#else
      auto file_path = dir + "/" + filename;
#endif
      if (CreateLogFile(file_path)) {
        current_logfile_path_ = file_path;
        create_file_success = true;
        // create symlink
#ifndef _WIN32
        auto link_path = dir + "/" + base_name_ + ".log";
        
        struct stat symlink_state;
        int status = lstat(link_path.c_str(), &symlink_state);
        if (status == 0) {
          unlink(link_path.c_str());
        }

        if (symlink(filename.c_str(), link_path.c_str()) != 0) {
          perror("Could not create symlink file");
        }
#endif
        break;
      }
    }

    if (!create_file_success) {
      perror("Could not create log file");
      fprintf(stderr, "COULD NOT CREATE LOGFILE '%s'!\n", filename.c_str());
      return;
    }

    // write a header
  }

  assert(file_ != nullptr);

  auto logstring = FormatAsText(r);
  logstring += "\n";
  size_t write_len = fwrite(logstring.data(), 1, logstring.length(), file_.get());
  if (write_len != logstring.length()) {
    perror("write log file failed");
    if (errno == ENOSPC) {
      fprintf(stderr, "DISK IS FULL!\n");
      // TODO: should we stop process more message while disk is full
    }
  }

  file_length_ += write_len;
  bytes_since_flush_ += write_len;

  if (bytes_since_flush_ > 4 * 1024) {
    FlushUnlocked();
  }

  if (file_length_ > max_file_length_) {
    CloseFileUnlocked();
  }
}

CPPLOG_INLINE bool FileSink::CreateLogFile(const std::string& file_name) {
#if defined(_WIN32)
  FILE* f = nullptr;
  if (fopen_s(&f, file_name.c_str(), "a+") != 0) {
    return false;
  }
#else
  FILE* f = fopen(file_name.c_str(), "a+");
  if (f == nullptr) {
    return false;
  }
#endif

  file_ = decltype(file_){f, fclose};
  bytes_since_flush_ = 0;
  file_length_ = 0;

  return true;
}

CPPLOG_INLINE void FileSink::CloseLogFile() {
  std::lock_guard<std::mutex> l(mutex_);
  CloseFileUnlocked();
}

CPPLOG_INLINE void FileSink::CloseFileUnlocked() {
  file_.reset(); // close current log file
  current_logfile_path_.clear();
  bytes_since_flush_ = 0;
  file_length_ = 0;
  rollover_attempt_ = kRolloverAttemptFrequency;
}

CPPLOG_INLINE void FileSink::FlushUnlocked() {
  assert(file_);

  fflush(file_.get());
  bytes_since_flush_ = 0;
}

} // namespace cpplog
