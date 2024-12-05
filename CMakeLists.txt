cmake_minimum_required(VERSION 3.28)
project(simple_tcp_implementation)

set(CMAKE_CXX_STANDARD 17)

add_executable(simple_tcp_implementation main.cpp
        header/ipv4/IPv4Header.h
        header/ipv4/IPv4Header.cpp
        header/tcp/TCPHeader.h
        header/tcp/TCPHeader.cpp
        byteop/extract/ByteExtractor.h
        byteop/extract/ByteExtractor.cpp
        tcb/State.h
        SimpleTCP.h
        SimpleTCP.cpp
        tcb/TransmissionControlBlock.h
        tcb/TransmissionControlBlock.cpp
        tcb/LocalConnection.h
        ReceiveParams.h
        byteop/insert/ByteInserter.h
        byteop/insert/ByteInserter.cpp)

if(WIN32)
    target_link_libraries(simple_tcp_implementation wsock32 ws2_32)
endif()