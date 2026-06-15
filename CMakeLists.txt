cmake_minimum_required(VERSION 3.10)

project(z80e C)

option(enable-sdl "Enables the SDL frontend" NO)
option(enable-tui "Enables the TUI frontend" YES)
option(enable-zex "Enables the zex test suite" NO)
option(enable-test "Enables the unit test frontend" NO)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")  

if(NOT WIN32 OR CYGWIN)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

if (MINGW OR MSYS)
  add_definitions(-DNOLINK)
  Message(STATUS "Building in MinGW or MSYS environment, disabling link socket")
endif()

if(APPLE)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAPPLE")
endif()

add_subdirectory(libz80e libz80e)
add_subdirectory(frontends/libz80e frontends/libz80e)

if(enable-test)
    add_subdirectory(frontends/test frontends/test)
endif()

if(NOT DEFINED EMSCRIPTEN)
  if(enable-tui)
    add_subdirectory(frontends/tui frontends/tui)
  endif()
  if(enable-sdl)
    add_subdirectory(frontends/sdl frontends/sdl)
  endif()
  if(enable-zex)
    add_subdirectory(frontends/zex frontends/zex)
  endif()
endif()
