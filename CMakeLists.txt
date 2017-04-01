cmake_minimum_required(VERSION 2.8)
project(cpplog CXX)

set(CMAKE_CXX_STANDARD "11")

set(CONAN_BUILD_FILE ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
if(EXISTS ${CONAN_BUILD_FILE})
 message("Using Conan")
 include(${CONAN_BUILD_FILE})
 conan_basic_setup()
 set(USE_CONAN_BUILD TRUE)
 set(JSON_INCLUDE_DIR ${CONAN_INCLUDE_DIRS_NLJSON})
 set(FMT_INCLUDE_DIR  ${CONAN_INCLUDE_DIRS_FMT})
endif()

if(NOT USE_CONAN_BUILD)
add_subdirectory(third_party/fmt)
set(FMT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt)

add_subdirectory(third_party/json)
set(JSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/src)
endif()

enable_testing()

add_subdirectory(test)
