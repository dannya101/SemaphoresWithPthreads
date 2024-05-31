cmake_minimum_required(VERSION 3.27)
project(Assignment4)

set(CMAKE_CXX_STANDARD 11)

add_executable(Assignment4 main.cpp
        Shared.h
        Shared.cpp
        log.cpp
        doordash.h
        doordash.cpp
)
