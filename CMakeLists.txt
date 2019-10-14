cmake_minimum_required(VERSION 3.0)

project(cpplog
  VERSION 0.0.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# if(CMAKE_PROJECT_NAME STREQUAL cpplog)
#   set(CPPLOG_IS_TOP_PROEJCT True)
# else()
#   set(CPPLOG_IS_TOP_PROEJCT False)
# endif()

### cpplog library
add_library(cpplog INTERFACE)
target_include_directories(cpplog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cpplog
  INTERFACE
    fmt
    spdlog::spdlog
)
