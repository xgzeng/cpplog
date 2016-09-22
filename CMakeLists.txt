cmake_minimum_required(VERSION 2.8)
project(cpplog CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# policy for IN_LIST operator
cmake_policy(SET CMP0057 NEW)
if(cxx_relaxed_constexpr IN_LIST CMAKE_CXX_COMPILE_FEATURES)
  add_definitions(-DCXX_RELAXED_CONSTEXPR)
endif()

set(CMAKE_CXX_STANDARD "14")

include_directories(".")

enable_testing()
add_subdirectory(test)
