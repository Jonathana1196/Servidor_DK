cmake_minimum_required(VERSION 3.5)
project(C_servidor C)

set(CMAKE_C_STANDARD 99)

add_executable(C_servidor main.c host.c host.h)
target_link_libraries( C_servidor${pthread} -lpthread)
