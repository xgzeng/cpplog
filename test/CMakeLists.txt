add_executable(cpplog_unittest
    cpplog_unittest.cpp
    file_sink_test.cpp
)

target_link_libraries(cpplog_unittest cpplog)

add_test(cpplog_unittest cpplog_unittest)
