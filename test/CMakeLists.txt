
include_directories(${JSON_INCLUDE_DIR} ${FMT_INCLUDE_DIR})
include_directories(..)

set(UNITTEST_SRCS cpplog_unittest.cpp)

add_executable(cpplog_unittest ${UNITTEST_SRCS})

target_link_libraries(cpplog_unittest fmt)

add_test(cpplog_unittest cpplog_unittest)

