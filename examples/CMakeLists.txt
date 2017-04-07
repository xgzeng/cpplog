include_directories(${JSON_INCLUDE_DIR} ${FMT_INCLUDE_DIR})
include_directories(..)

add_executable(udpsink_to_logstash
    udpsink_to_logstash.cpp
)

target_link_libraries(udpsink_to_logstash fmt)


