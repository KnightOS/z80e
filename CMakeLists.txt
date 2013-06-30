cmake_minimum_required(VERSION 2.8.5)
project(z80e)

add_executable(z80e
    core/cpu.c
    ti/asic.c
    ti/memory.c
    main.c
)

include_directories(
    core/
    ti/
)
