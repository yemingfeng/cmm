cmake_minimum_required(VERSION 3.13)
project(cmm)

set(CMAKE_CXX_STANDARD 11)

add_executable(cmm main.cpp cmm/cmmpool.h)