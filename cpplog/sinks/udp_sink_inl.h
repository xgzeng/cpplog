#pragma once
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_addr
#include "fmt/time.h"

namespace cpplog {

CPPLOG_INLINE UdpSink::UdpSink(string_view addr) {
  SocketHandle sock(socket(PF_INET, SOCK_DGRAM, 0));
  if (!sock) {
    throw std::system_error(errno, std::generic_category());
  }

  // parse addr
  std::string ip;
  uint16_t port;

  auto colon_pos = addr.find_first_of(':');
  if (colon_pos == string_view::npos) {
    ip = addr;
    port = DEFAULT_PORT;
  } else {
    ip = addr.substr(0, colon_pos);
    port = std::stoi(addr.substr(colon_pos + 1));
  }

  // invalid port
  if (port == 0) {
    throw std::invalid_argument("invalid port number");
  }

  sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = inet_addr(ip.c_str());

  if (connect(sock.get(), (const sockaddr*)&saddr, sizeof(saddr)) != 0) {
    throw std::system_error(errno, std::generic_category());
  }

  sock_ = std::move(sock);
}

CPPLOG_INLINE std::string to_iso8601(timespec ts) {
  struct tm result;
  gmtime_r(&ts.tv_sec, &result);
  return fmt::format("{:%FT%T}.{:03}Z", result, ts.tv_nsec/1000/1000); //  / (1000 * 1000)
}

CPPLOG_INLINE void UdpSink::SubmitRecord(const LogRecord& record) {
  json j = record.fields();
  j["@timestamp"] = to_iso8601(record.timestamp());
  j["@version"] = "1";
  j["level"] = record.level();
  j["message"] = record.message();
  j["src_file_info"] = record.src_file_info();
  std::string s = j.dump();
  send(sock_.get(), s.data(), s.size(), 0);
}

} // namespace cpplog

