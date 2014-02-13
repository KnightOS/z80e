cmake_minimum_required(VERSION 2.8.5)
project(z80e)
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

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
