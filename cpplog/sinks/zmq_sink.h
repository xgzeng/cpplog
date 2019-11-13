#pragma once
#include "cpplog/sink.h"
#include "cpplog/formatter/json_formatter.h"
#include <zmq.hpp>
#include <initializer_list>

namespace cpplog {

class ZeroMQSink : public LogSink {
public:
  ZeroMQSink();
  ZeroMQSink(const char* endpoint);
  ZeroMQSink(const std::initializer_list<const char*> &endpoints);

  void Bind(const char* endpoint);

  void Submit(const LogRecord&) override;

private:
  zmq::context_t zmq_ctx_;
  zmq::socket_t zmq_pub_sock_;
};

ZeroMQSink::ZeroMQSink()
: zmq_pub_sock_(zmq_ctx_, zmq::socket_type::pub) {
}

CPPLOG_INLINE ZeroMQSink::ZeroMQSink(const char* endpoint)
: ZeroMQSink() {
  Bind(endpoint);
}

CPPLOG_INLINE ZeroMQSink::ZeroMQSink(
    const std::initializer_list<const char*> &endpoints)
: zmq_pub_sock_(zmq_ctx_, zmq::socket_type::pub) {
  for (auto e : endpoints) {
    Bind(e);
  }
}

CPPLOG_INLINE void ZeroMQSink::Bind(const char* endpoint) {
  zmq_pub_sock_.bind(endpoint);
}

CPPLOG_INLINE void ZeroMQSink::Submit(const LogRecord& record) {
  std::string s = FormatAsJSON(record);
  zmq_pub_sock_.send(s.data(), s.size());
}

} // namespace cpplog
