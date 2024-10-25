cmake_minimum_required(VERSION 3.29)
project(P1 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(P0
        list.c
    list.h
        p1.c)
