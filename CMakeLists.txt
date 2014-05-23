cmake_minimum_required(VERSION 2.8.5)
project(z80e C)
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
add_definitions("-Wall")

add_executable(test
    core/cpu.c
    core/registers.c
    ti/asic.c
    ti/memory.c
    test.c
)

add_executable(z80e
    core/cpu.c
    core/registers.c
    ti/asic.c
    ti/memory.c
    main.c
)

include_directories(
    core/
    ti/
)

set(CMAKE_BUILD_TYPE Release)
