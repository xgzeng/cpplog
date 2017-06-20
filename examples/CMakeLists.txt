add_executable(udpsink_to_logstash
    udpsink_to_logstash.cpp
)

target_link_libraries(udpsink_to_logstash cpplog)

if(WIN32)
	target_link_libraries(udpsink_to_logstash Ws2_32)
endif()
