add_executable(cpplog_unittest
    cpplog_unittest.cpp
    file_sink_test.cpp
    udp_sink_test.cpp
    nlohmann_json_test.cpp
)

target_link_libraries(cpplog_unittest cpplog)

add_test(cpplog_unittest cpplog_unittest)
