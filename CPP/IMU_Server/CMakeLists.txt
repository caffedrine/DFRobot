cmake_minimum_required(VERSION 3.7)
project(IMU_Server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp tcpserver.cpp tcpserver.h)
add_executable(IMU_Server ${SOURCE_FILES})