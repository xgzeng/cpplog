cmake_minimum_required(VERSION 2.8)
project(cpplog CXX)

option(CPPLOG_BUILD_TESTS "Build unit tests" ON)
option(CPPLOG_BUILD_EXAMPLES "Build examples" ON)

set(CMAKE_CXX_STANDARD "11")

set(CONAN_BUILD_FILE ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
if(EXISTS ${CONAN_BUILD_FILE})
    message("Using Conan")
    include(${CONAN_BUILD_FILE})
    conan_basic_setup()
    set(USE_CONAN_BUILD TRUE)
endif()

if(NOT USE_CONAN_BUILD)
    if(NOT TARGET fmt)
      add_subdirectory(third_party/fmt)
    endif()

    if(NOT TARGET nlohmann_json)
        set(BuildTests OFF)
        add_subdirectory(third_party/json)
    endif()
endif()

### cpplog library
add_library(cpplog INTERFACE)
target_include_directories(cpplog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cpplog INTERFACE fmt nlohmann_json)

### Examples
if (CPPLOG_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

### Tests
if (CPPLOG_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(CPPLOG_BUILD_TESTS)
