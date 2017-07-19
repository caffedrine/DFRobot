cmake_minimum_required(VERSION 3.7)
project(IMU_Server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(IMU_Server ${SOURCE_FILES})