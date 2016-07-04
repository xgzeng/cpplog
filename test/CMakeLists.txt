set(UNITTEST_SRCS
    cpplog_unittest.cpp
    json_builder_unittest.cpp
)

if(NLOHMANN_JSON_FOUND)
  include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
  set(UNITTEST_SRCS ${UNITTEST_SRCS} support_nlohmann_json_unittest.cpp)
endif()

add_executable(cpplog_unittest ${UNITTEST_SRCS})

target_link_libraries(cpplog_unittest ${CPPFORMAT_LIBRARIES})

add_test(cpplog_unittest cpplog_unittest)

