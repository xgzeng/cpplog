#include "catch.hpp"
#include <cpplog/sinks/udp_sink.h>

using namespace cpplog;

TEST_CASE("UdpSink") {
  // open socket to receive log message
  auto fd = socket(PF_INET, SOCK_DGRAM, 0);
  REQUIRE(fd != -1);
  
  sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(UdpSink::DEFAULT_PORT);

  auto ret =bind(fd, (const sockaddr*) &saddr, sizeof(saddr));
  REQUIRE(ret == 0);
  
  // 
  UdpSink sink("127.0.0.1");
  LogRecord r;
  r.set_message("test log message");
  sink.Submit(r);
  
  char buf[256] = {};
  ret = recv(fd, buf, sizeof(buf), 0);
  REQUIRE(ret != -1);
  
  auto j = json::parse(std::string(buf, ret));
  REQUIRE(j["message"] == "test log message");
}
