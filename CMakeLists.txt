cmake_minimum_required(VERSION 3.23)
project(SkipList)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread" )

add_executable(SkipList main.cpp SkipList.h)
