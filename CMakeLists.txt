cmake_minimum_required(VERSION 2.8.5)
project(z80e C)

add_subdirectory(libz80e libz80e)
add_subdirectory(frontends/test frontends/test)

set(CMAKE_BUILD_TYPE Release)
