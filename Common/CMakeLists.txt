cmake_minimum_required(VERSION 3.2)

project(common VERSION 0.0.1)

#protobuf
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
#find .bp.h
include_directories(${CMAKE_CURRENT_BINARY_DIR})
#gen protobuf files
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS pack.proto)

add_library(common STATIC common.cpp ${PROTO_HDRS} ${PROTO_SRCS})

target_include_directories(common PUBLIC 
                                        ${CMAKE_CURRENT_SOURCE_DIR} 
                                        ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(common PUBLIC ${Protobuf_LIBRARIES})