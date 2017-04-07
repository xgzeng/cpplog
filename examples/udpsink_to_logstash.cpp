#include "cpplog/logging.h"
#include "cpplog/sinks/udp_sink.h"

using namespace cpplog;

int main(int argc, char* argv[]) {
  printf("Start logstash at localhost with following command to receive log event:\n"
         "logstash -e \"input { udp { port => 1234 codec => json {} } } output { stdout {}}\"\n\n");

  LogDispatcher::instance().AddSink<UdpSink>("127.0.0.1:1234");

  for (int i = 0; i < 1000; ++i) {
    LOG(INFO, "message {}", i);
    sleep(5); // seconds
  }
  return 0;
}

