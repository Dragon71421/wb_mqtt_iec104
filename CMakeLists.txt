cmake_minimum_required (VERSION 2.8.11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

include_directories(
                src
                src/loggers
)

set (bin_src    src/main.cpp
                src/config_parser.cpp
                src/loggers/mqtt_logger.cpp
                src/iec104_server.cpp
                src/mqtt_client.cpp
                src/mqtt_iec104_converter.cpp
)

set (libs       wbmqtt
                jsoncpp
                mosquittopp
                lib60870
)


# create main executed file
add_executable( iec104-server ${bin_src} )

target_link_libraries( iec104-server ${libs} )
