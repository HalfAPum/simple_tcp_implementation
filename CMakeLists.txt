cmake_minimum_required(VERSION 3.28)
project(simple_tcp_implementation)

set(CMAKE_CXX_STANDARD 17)

add_executable(simple_tcp_implementation main.cpp)

if(WIN32)
    target_link_libraries(simple_tcp_implementation wsock32 ws2_32)
endif()