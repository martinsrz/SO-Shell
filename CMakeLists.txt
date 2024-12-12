cmake_minimum_required(VERSION 3.29)
project(P3 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(P3
        list.c
    list.h
        p3.c
        memoryList.c
        memoryList.h
        processesList.c
        processesList.h
        directoriesList.c
        directoriesList.h
        memory.c
        memory.h
        processes.c
        processes.h)
