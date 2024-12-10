project(SimpleTCP)

set(HEADER_FILES
        header/ipv4/IPv4Header.h
        header/tcp/TCPHeader.h
        byteop/extract/ByteExtractor.h
        tcb/State.h
        SimpleTCP.h
        tcb/TransmissionControlBlock.h
        tcb/LocalConnection.h
        ReceiveParams.h
        byteop/insert/ByteInserter.h
        Constants.h
        header/udp/UDPHeader.h
        socket/SocketFactory.h
        facade/TCPFacade.h
)

set(SOURCE_FILES
        main.cpp
        header/ipv4/IPv4Header.cpp
        header/tcp/TCPHeader.cpp
        byteop/extract/ByteExtractor.cpp
        SimpleTCP.cpp
        tcb/TransmissionControlBlock.cpp
        byteop/insert/ByteInserter.cpp
        header/udp/UDPHeader.cpp
        tcb/LocalConnection.cpp
        socket/SocketFactory.cpp
        facade/TCPFacade.cpp
)

add_library(SimpleTCP_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})

if(WIN32)
    target_link_libraries(SimpleTCP_lib wsock32 ws2_32)
endif()