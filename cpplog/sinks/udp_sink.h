#pragma once
#include "cpplog/sink.h"
#include "cpplog/sinks/socket_helper.h"

namespace cpplog {

class UdpSink : public LogSink {
public:
  static const uint16_t DEFAULT_PORT = 4567;

  UdpSink(string_view receiver_addr);

  void Submit(const LogRecord&) override;

private:
  SocketHandle sock_;
};

} // namespace cpplog

#include "cpplog/sinks/udp_sink_inl.h"
