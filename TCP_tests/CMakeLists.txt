Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(TCP_tests_run TCPHandshakeTest.cpp
        facade/TCPFacadeMock.cpp
        facade/TCPFacadeMock.h)

target_link_libraries(TCP_tests_run PRIVATE Catch2::Catch2WithMain)

target_link_libraries(TCP_tests_run PRIVATE SimpleTCP_lib)