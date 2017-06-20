set(UNITTEST_SRCS cpplog_unittest.cpp)

add_executable(cpplog_unittest ${UNITTEST_SRCS})

target_link_libraries(cpplog_unittest cpplog)

add_test(cpplog_unittest cpplog_unittest)
