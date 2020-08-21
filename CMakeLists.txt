cmake_minimum_required(VERSION 3.15)
project(myshell)

set(CMAKE_CXX_STANDARD 17)

add_executable(myshell main.cpp interpreter.cpp ENV.cpp)
