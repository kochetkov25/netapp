cmake_minimum_required(VERSION 3.2)

project(server VERSION 0.0.1)

add_executable(server TCPServer.hpp
                      TCPServer.cpp
                      main.cpp)

#link common
target_link_libraries(server PRIVATE common)
#common headers dir
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/Common)