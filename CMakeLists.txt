cmake_minimum_required(VERSION 2.8.5)
project(z80e C)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

add_subdirectory(libz80e libz80e)
add_subdirectory(frontends/libz80e frontends/libz80e)
add_subdirectory(frontends/test frontends/test)

if(NOT DEFINED EMSCRIPTEN)
add_subdirectory(frontends/tui frontends/tui)
endif()

set(CMAKE_BUILD_TYPE Release)
