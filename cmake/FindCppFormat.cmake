find_file(CPPFORMAT_INCLUDE_DIR cppformat/format.h )
find_library(CPPFORMAT_LIBRARY cppformat)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(CPPFORMAT
    REQUIRED_VARS CPPFORMAT_INCLUDE_DIR CPPFORMAT_LIBRARY)

#if(NOT CppFormat_FOUND)
#endif()
