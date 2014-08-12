cmake_minimum_required(VERSION 2.8.5)
project(z80e C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

add_subdirectory(libz80e ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libz80e)
add_subdirectory(frontends/test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/frontends/test)

set(CMAKE_BUILD_TYPE Release)
