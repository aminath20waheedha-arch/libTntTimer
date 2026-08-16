cmake_minimum_required(VERSION 3.18)
project(TNTTimer)

set(CMAKE_CXX_STANDARD 20)

add_library(
    TNTTimer
    SHARED
    src/main.cpp
)

find_library(log-lib log)

target_link_libraries(
    TNTTimer
    ${log-lib}
)
