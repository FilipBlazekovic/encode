cmake_minimum_required(VERSION 3.27)
project(encode C)

set(CMAKE_C_STANDARD 17)

add_library(encode STATIC src/encode.h src/encode.c)

target_link_libraries(encode m)
