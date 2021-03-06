#include "cpplog/logging.h"
#include "cpplog/sinks/file_sink.h"
#include <thread>

#ifdef _WIN32
#undef max // max macro conflict with catch
#endif
#include "catch.hpp"

using cpplog::FileSink;
using cpplog::LogRecord;

TEST_CASE("FileSink Constructor") {
  FileSink s1("test");
  FileSink s2(std::string("test"));
}

static bool FileExists(const std::string& path) {
#ifdef _WIN32
  FILE* fp = nullptr;
  if (fopen_s(&fp, path.c_str(), "r") == 0) {
    fclose(fp);
    return true;
  } else {
    return false;
  }
#else
  FILE* fp = fopen(path.c_str(), "r");
  if (fp != nullptr) {
    fclose(fp);
    return true;
  } else {
    return false;
  }
#endif
}

TEST_CASE("FileSink CreateLogFile") {
  FileSink s1("test");
  REQUIRE(s1.current_logfile_path().empty());

  s1.Submit(LogRecord());

  auto logfile_path = s1.current_logfile_path();
  REQUIRE(!logfile_path.empty());

  s1.CloseLogFile();
  REQUIRE(s1.current_logfile_path().empty());

  REQUIRE(FileExists(logfile_path));

  // remove test output
  REQUIRE(remove(logfile_path.c_str()) == 0);

  FileSink s3;
  REQUIRE(s3.base_name() == "cpplog_unittest");
}

TEST_CASE("FileSink LogFile Rotation") {
  FileSink s("test");
  s.set_max_file_length(2000); // 2K

  s.Submit(LogRecord());
  auto logfile_path_1 = s.current_logfile_path();

  // sleep 1 second to ensure log file will rollover
  std::this_thread::sleep_for(std::chrono::seconds(1));

  for (int i = 0; i < 2000; ++i) {
    LogRecord r;
    r.set_message(fmt::format("{}", i));
    s.Submit(r);
  }

  auto logfile_path_2 = s.current_logfile_path();
  s.CloseLogFile();

  REQUIRE(!logfile_path_1.empty());
  REQUIRE(!logfile_path_2.empty());
  REQUIRE(logfile_path_1 != logfile_path_2);
  REQUIRE(FileExists(logfile_path_1));
  REQUIRE(FileExists(logfile_path_2));

  // remove test output
  REQUIRE(remove(logfile_path_1.c_str()) == 0);
  REQUIRE(remove(logfile_path_2.c_str()) == 0);
}
