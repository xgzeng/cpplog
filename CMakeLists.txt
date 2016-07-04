cmake_minimum_required(VERSION 2.8)
project(cpplog)

set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake-modules")

set(CMAKE_CXX_STANDARD "14")

include("UseSubmodule")

UseSubmodule(CppFormat "third_party/cppformat")
UseSubmodule(NlohmannJson "third_party/nlohmann_json/")

include_directories("." "${CPPFORMAT_INCLUDE_DIR}")

enable_testing()
add_subdirectory(test)
