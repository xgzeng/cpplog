
if(TARGET cppzmq-static)
  set(ZMQ_FOUND TRUE)
endif()

if(ZMQ_FOUND)
  add_executable(zmqlogdump zmqlogdump.cpp)
  target_link_libraries(zmqlogdump cppzmq-static nlohmann_json)
endif()
