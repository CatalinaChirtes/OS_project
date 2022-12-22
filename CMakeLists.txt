cmake_minimum_required(VERSION 3.24)
project(terminalOS)

set(CMAKE_CXX_STANDARD 17)

add_executable(terminalOS main.cpp)
add_executable(dirname dirname.cpp)
add_executable(tee tee.cpp)
add_executable(cp cp.cpp)

target_link_libraries(terminalOS /lib/x86_64-linux-gnu/libreadline.so)
