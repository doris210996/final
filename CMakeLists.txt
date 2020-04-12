cmake_minimum_required(VERSION 3.14)
project(ex2_os)

set(CMAKE_CXX_STANDARD 17)

add_executable(ex2_os Thread.cpp Thread.h uthreads.h uthreads.cpp)