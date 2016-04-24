cmake_minimum_required(VERSION 2.8)
project(cpplog)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD "14")

include("UseCppFormat")
include("UseNlohmannJson")

include_directories("." "${CPPFORMAT_INCLUDE_DIR}")

enable_testing()
add_subdirectory(test)
