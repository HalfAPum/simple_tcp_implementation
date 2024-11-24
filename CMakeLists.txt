cmake_minimum_required(VERSION 3.28)
project(simple_tcp_implementation)

set(CMAKE_CXX_STANDARD 17)

add_executable(simple_tcp_implementation main.cpp
        header/IPv4Header.h
        header/IPv4Header.cpp
        header/TCPHeader.h
        header/TCPHeader.cpp
        ByteExtractor.h
        ByteExtractor.cpp
        State.h
        SimpleTCP.h
        SimpleTCP.cpp
        TransmissionControlBlock.h
        TransmissionControlBlock.cpp
        LocalConnection.h
        ReceiveParams.h)

if(WIN32)
    target_link_libraries(simple_tcp_implementation wsock32 ws2_32)
endif()