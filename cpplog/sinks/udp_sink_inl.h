#pragma once

#include <cpplog/nlohmann_json.h>

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_pton
#endif

#include <fmt/time.h>

#ifdef _WIN32
#define THROW_SOCKET_SYSTEM_ERROR() \
  throw std::system_error(WSAGetLastError(), std::generic_category())
#else
#define THROW_SOCKET_SYSTEM_ERROR() \
  throw std::system_error(errno, std::generic_category())
#endif

namespace cpplog {

CPPLOG_INLINE UdpSink::UdpSink(string_view addr) {
  SocketHandle sock(socket(PF_INET, SOCK_DGRAM, 0));
  if (!sock) {
    THROW_SOCKET_SYSTEM_ERROR();
  }

  // parse addr
  std::string ip;
  uint16_t port;

  auto colon_pos = addr.find_first_of(':');
  if (colon_pos == string_view::npos) {
    ip = std::string{addr};
    port = DEFAULT_PORT;
  } else {
    ip = std::string{addr.substr(0, colon_pos)};
    port = std::stoi(std::string{addr.substr(colon_pos + 1)});
  }
  
  if (ip.empty()) {
    ip = "127.0.0.1";
  }

  // invalid port
  if (port == 0) {
    throw std::invalid_argument("invalid port number");
  }

  sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);

#ifdef _WIN32
  if (!InetPton(AF_INET, ip.c_str(), &saddr.sin_addr.s_addr)) {
    THROW_SOCKET_SYSTEM_ERROR();
  }
#else
  if (!inet_pton(AF_INET, ip.c_str(), &saddr.sin_addr.s_addr)) {
    THROW_SOCKET_SYSTEM_ERROR();
  }
#endif

  if (connect(sock.get(), (const sockaddr*)&saddr, sizeof(saddr)) != 0) {
    THROW_SOCKET_SYSTEM_ERROR();
  }

  sock_ = std::move(sock);
}

CPPLOG_INLINE std::string to_iso8601(timespec ts) {
  struct tm result;
#ifdef _WIN32
  gmtime_s(&result, &ts.tv_sec);
#else
  gmtime_r(&ts.tv_sec, &result);
#endif
  return fmt::format("{:%FT%T}.{:03}Z", result, ts.tv_nsec/1000/1000); //  / (1000 * 1000)
}

CPPLOG_INLINE void UdpSink::Submit(const LogRecord& record) {
  json j{};
  // = record.fields();
  j["@timestamp"] = to_iso8601(record.timestamp());
  j["@version"] = "1";
  j["level"] = record.level();
  j["message"] = record.message();
  j["source_location"] = record.src_location();
  std::string s = j.dump();
  send(sock_.get(), s.data(), s.size(), 0);
}

} // namespace cpplog
