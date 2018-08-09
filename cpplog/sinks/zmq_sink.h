#pragma once
#include "cpplog/sink.h"
#include "cpplog/formatter/json_formatter.h"
#include <zmq.h>

namespace cpplog {

class ZeroMQSink : public LogSink {
public:
  ZeroMQSink(const char* endpoint);

  ~ZeroMQSink();

  void Submit(const LogRecord&) override;

private:
  void* zmq_ctx_ = nullptr;
  void* zmq_pub_sock_ = nullptr;
};

CPPLOG_INLINE ZeroMQSink::ZeroMQSink(const char* endpoint) {
  zmq_ctx_ = zmq_ctx_new();
  assert(zmq_ctx_);
  zmq_pub_sock_ = zmq_socket(zmq_ctx_, ZMQ_PUB);
  assert(zmq_pub_sock_);

  if (!zmq_bind(zmq_pub_sock_, endpoint)) {
    // release resource
    zmq_close(zmq_pub_sock_);
    zmq_ctx_term(zmq_ctx_);

    throw std::system_error(errno, std::system_category());
  }
}

CPPLOG_INLINE ZeroMQSink::~ZeroMQSink() {
  int ret = zmq_close(zmq_pub_sock_);
  assert(ret == 0);
  ret = zmq_ctx_term(zmq_ctx_);
  assert(ret == 0);
}

CPPLOG_INLINE void ZeroMQSink::Submit(const LogRecord& record) {
  auto s = FormatAsJSON(record);
  zmq_send(zmq_pub_sock_, s.data(), s.size(), 0);
}

} // namespace cpplog
