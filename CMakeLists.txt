cmake_minimum_required (VERSION 2.8.11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

# create main executed file
add_executable(main src/main.cpp)

target_link_libraries(main wbmqtt jsoncpp mosquittopp)
