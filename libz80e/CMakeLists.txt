cmake_minimum_required(VERSION 2.6)
project (z80e_objects)

FILE(GLOB Hardware src/ti/hardware/*.c)
FILE(GLOB Commands src/debugger/commands/*.c)

add_library(z80e_objects OBJECT
    src/core/cpu.c
    src/core/registers.c
    src/debugger/debugger.c
    src/debugger/hooks.c
    src/disassembler/disassemble.c
    src/ti/asic.c
    src/ti/memory.c
    src/log/log.c
    src/runloop/runloop.c
    ${Hardware}
    ${Commands}
)

set_target_properties(z80e_objects PROPERTIES APPEND_STRING PROPERTY
    COMPILE_FLAGS " ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX} ${CMAKE_CURRENT_SOURCE_DIR}/include/")

set_target_properties(z80e_objects PROPERTIES APPEND_STRING PROPERTY
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/z80e")
