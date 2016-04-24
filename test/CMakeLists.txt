include_directories(${NLOHMANN_JSON_INCLUDE_DIR})

add_executable(cpplog_unittest
               cpplog_unittest.cpp
               json_builder_unittest.cpp
               support_nlohmann_json_unittest.cpp)

target_link_libraries(cpplog_unittest ${CPPFORMAT_LIBRARY})

add_test(cpplog_unittest cpplog_unittest)

