cmake_minimum_required(VERSION 3.2)

project(client VERSION 0.0.1)

add_executable(client TCPClient.hpp
                      TCPClient.cpp
                      main.cpp)

#link common
target_link_libraries(client PRIVATE common)
#common headers dir
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/Common)