cmake_minimum_required (VERSION 2.8.11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

# create main executed file
add_executable( wb-mqtt-iec104 src/main.cpp src/config_parser.cpp )

target_link_libraries( wb-mqtt-iec104 wbmqtt jsoncpp mosquittopp )
