cmake_minimum_required(VERSION 2.8)
project(cpplog)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD "14")

include_directories(".")

enable_testing()
add_subdirectory(test)
