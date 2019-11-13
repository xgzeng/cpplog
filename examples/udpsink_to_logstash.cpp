#include "cpplog/logging.h"
#include "cpplog/sinks/udp_sink.h"

#include <thread>

using namespace cpplog;

int main(int argc, char* argv[]) {
#ifdef _WIN32
  WSADATA wsaData = { 0 };
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    printf("WSAStartup failed\n");
    return 1;
  }
#endif

  printf("Start logstash at localhost with following command to receive log event:\n"
         "logstash -e \"input { udp { port => 1234 codec => json {} } } output { stdout {}}\"\n\n");

  LogDispatcher::instance().AddSink<UdpSink>("127.0.0.1:1234");

  for (int i = 0; i < 1000; ++i) {
    LOG(INFO, "message {}", i);
    std::this_thread::sleep_for(std::chrono::seconds(5)); // seconds
  }
  return 0;
}
