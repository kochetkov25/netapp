cmake_minimum_required(VERSION 3.2)

project(netapp VERSION 0.0.1)

#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Common)
add_subdirectory(Server)
add_subdirectory(Client)