#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s endpoint\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char* zmq_endpoint = argv[1];

  zmq::context_t zmq_context;
  zmq::socket_t zmq_socket(zmq_context, zmq::socket_type::sub);

  zmq_socket.connect(zmq_endpoint);
  zmq_socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

  while (true) {
    zmq::message_t zmq_msg;
    zmq_socket.recv(&zmq_msg);

    const char* p_data = reinterpret_cast<const char*>(zmq_msg.data());

    try {
      auto msg = nlohmann::json::parse(p_data, p_data + zmq_msg.size());
      auto timestamp = msg.value("@timestamp", "");
      auto level = msg.value("level", "");
      std::string message = msg.value("message", "");
      std::cout << level << ":" << message << std::endl;
    } catch(std::exception& e) {
      std::cerr << "process message error: " << e.what() << std::endl;
    }
  }

  return EXIT_SUCCESS;
}
