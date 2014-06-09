cmake_minimum_required(VERSION 2.8.5)
project(z80e C)
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
add_definitions("-Wall")

FILE(GLOB Hardware ti/hardware/*.c)

add_executable(test
    core/cpu.c
    core/registers.c
    debugger/debugger.c
    ti/asic.c
    ti/memory.c
    ${Hardware}
    test.c
)

add_executable(z80e
    core/cpu.c
    core/registers.c
    debugger/debugger.c
    ti/asic.c
    ti/memory.c
    ${Hardware}
    main.c
)

include_directories(
    core/
    debugger/
    ti/
    ti/hardware/
)

set(CMAKE_BUILD_TYPE Release)
