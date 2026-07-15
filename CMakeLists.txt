cmake_minimum_required(VERSION 4.3)
project(c_server)
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")
include_directories(include)
add_executable(main ${SOURCES})

