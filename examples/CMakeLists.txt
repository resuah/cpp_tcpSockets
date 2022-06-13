cmake_minimum_required(VERSION 3.10.0)
project(socketTest VERSION 0.1.0)


find_package(Threads REQUIRED)


## server x86
add_executable(testSocket_server testSocket_server.cpp)
target_link_libraries(testSocket_server PRIVATE Threads::Threads )

set_target_properties(testSocket_server PROPERTIES CXX_STANDARD 20)
set_target_properties(testSocket_server PROPERTIES CXX_STANDARD_REQUIRED ON)


## client x86
add_executable(testSocket_client testSocket_client.cpp)
target_link_libraries(testSocket_client PRIVATE Threads::Threads ${CURSES_LIBRARIES})

set_target_properties(testSocket_client PROPERTIES CXX_STANDARD 20)
set_target_properties(testSocket_client PROPERTIES CXX_STANDARD_REQUIRED ON)


