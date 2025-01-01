cmake_minimum_required(VERSION 3.2)

project(common VERSION 0.0.1)

add_library(common STATIC common.cpp)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})