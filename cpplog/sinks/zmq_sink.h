#pragma once
#include "cpplog/sink.h"
#include "cpplog/formatter/json_formatter.h"
#include <zmq.hpp>

namespace cpplog {

class ZeroMQSink : public LogSink {
public:
  ZeroMQSink(const char* endpoint);

  void Submit(const LogRecord&) override;

private:
  zmq::context_t zmq_ctx_;
  zmq::socket_t zmq_pub_sock_;
};

CPPLOG_INLINE ZeroMQSink::ZeroMQSink(const char* endpoint)
: zmq_pub_sock_(zmq_ctx_, zmq::socket_type::pub) {
  zmq_pub_sock_.bind(endpoint);
}

CPPLOG_INLINE void ZeroMQSink::Submit(const LogRecord& record) {
  std::string s = FormatAsJSON(record);
  zmq_pub_sock_.send(s.data(), s.size());
}

} // namespace cpplog
