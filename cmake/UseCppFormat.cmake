if(NOT CppFormat_FOUND)
  find_package(CppFormat QUIET)
endif()

if(NOT CppFormat_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cppformat")
  message("No cppformat found in system, try to use embedded cppformat")
  add_subdirectory("third_party/cppformat")
  set(CPPFORMAT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cppformat")
  set(CPPFORMAT_LIBRARY cppformat)
  set(CppFormat_FOUND TRUE)
endif()

if(NOT CppFormat_FOUND)
  message(FATAL_ERROR "cppformat is required to use cpplog")
else()
  message(STATUS "cppformat is found at ${CPPFORMAT_INCLUDE_DIR}")
endif()

