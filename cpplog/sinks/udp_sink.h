#pragma once

#include "cpplog/sink.h"

#ifdef WIN32
#include <WinSock2.h>
#else
#include <unistd.h> // close
#endif

namespace cpplog {

class SocketHandle {
#ifdef WIN32
  typedef SOCKET HANDLE_TYPE;
  static const HANDLE_TYPE INVALID_VALUE = INVALID_SOCKET;
#else
  typedef int HANDLE_TYPE;
  static const HANDLE_TYPE INVALID_HANDLE = -1;
#endif

public:
  SocketHandle() : fd_(INVALID_VALUE) {}

  SocketHandle(HANDLE_TYPE fd) : fd_(fd) {}

  ~SocketHandle() {
#ifdef WIN32
    if (fd_ != INVALID_VALUE) closesocket(fd_);
#else
    if (fd_ != INVALID_VALUE) close(fd_);
#endif
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
    other.fd_ = INVALID_VALUE;
  }

  SocketHandle& operator=(SocketHandle&& other) {
    SocketHandle tmp;
    swap(tmp);
    swap(other);
    return *this;
  }

  explicit operator bool () const {
    return fd_ != INVALID_VALUE;
  }

  HANDLE_TYPE get() const {
    return fd_;
  }

private:
  HANDLE_TYPE fd_;
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

