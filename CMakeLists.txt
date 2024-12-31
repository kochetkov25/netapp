cmake_minimum_required(VERSION 3.2)

project(netapp VERSION 0.0.1)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS system asio)



add_executable(netapp src/main.cpp
                      header/TCPClient.hpp
                      src/TCPClient.cpp
                      header/TCPServer.hpp
                      src/TCPServer.cpp
                      header/common.hpp
                      src/common.cpp)


target_include_directories(netapp PUBLIC header)
