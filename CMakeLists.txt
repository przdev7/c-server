cmake_minimum_required(VERSION 4.3)
project(c_server)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")
include_directories(include)
add_executable(main ${SOURCES})

