#pragma once

#include "cpplog/sink.h"

#ifndef WIN32
#include <unistd.h> // close
#endif

namespace cpplog {

class SocketHandle {
public:
  SocketHandle() : fd_(-1) {}

  SocketHandle(int fd) : fd_(fd) {}

  ~SocketHandle() {
    if (fd_ != -1) close(fd_);
  }

  // uncopyable
  SocketHandle(const SocketHandle&) = delete;
  SocketHandle& operator=(const SocketHandle&) = delete;

  // swapable
  void swap(SocketHandle& other) {
    std::swap(fd_, other.fd_);
  }

  // moveable
  SocketHandle(SocketHandle&& other)
  : fd_(other.fd_) {
    other.fd_ = -1;
  }

  SocketHandle& operator=(SocketHandle&& other) {
    SocketHandle tmp;
    swap(tmp);
    swap(other);
    return *this;
  }

  explicit operator bool () const {
    return fd_ != -1;
  }

  int get() const {
    return fd_;
  }

private:
  int fd_;
};

class UdpSink : public LogSink {
public:
  const uint16_t DEFAULT_PORT = 4567;

  UdpSink(string_view receiver_addr);

  void SubmitRecord(const LogRecord&) override;

private:
  SocketHandle sock_;
};

} // namespace cpplog

#include "cpplog/sinks/udp_sink_inl.h"

