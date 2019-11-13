cmake_minimum_required(VERSION 3.0)

project(cpplog
  VERSION 0.0.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(CPPLOG_BUILD_TESTS "Build unit tests" ON)
option(CPPLOG_BUILD_EXAMPLES "Build examples" ON)

if(CMAKE_PROJECT_NAME STREQUAL cpplog)
  set(CPPLOG_IS_TOP_PROEJCT True)
else()
  set(CPPLOG_IS_TOP_PROEJCT False)
endif()

if(NOT CPPLOG_IS_TOP_PROEJCT)
  if(NOT TARGET fmt)
    message(FATAL_ERROR "fmt library is required")
  endif()

  if(NOT TARGET nlohmann_json)
    message(FATAL_ERROR "nlohmann_json library is required")
  endif()
else()
  # use submodule fmt
  add_subdirectory(third_party/fmt)

  # use submodule nlohmann_json
  set(BuildTests OFF)
  add_subdirectory(third_party/json)
  unset(BuildTests)
endif()

### cpplog library
add_library(cpplog INTERFACE)
target_include_directories(cpplog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cpplog
  INTERFACE
    fmt
    nlohmann_json
    # link to pthread is required to use std::call_once
    pthread
)

add_subdirectory(cpplog)

### Examples
if (CPPLOG_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

link_libraries(cpplog)

### Tests
if (CPPLOG_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(CPPLOG_BUILD_TESTS)

# Tools
add_subdirectory(tools)
