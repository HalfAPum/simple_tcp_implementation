cmake_minimum_required(VERSION 3.28)
project(simple_tcp_implementation)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp)
add_executable(SimpleTCP_run ${SOURCE_FILES})

include_directories(SimpleTCP_lib)
add_subdirectory(SimpleTCP_lib)

target_link_libraries(SimpleTCP_run SimpleTCP_lib)

add_subdirectory(TCP_tests)