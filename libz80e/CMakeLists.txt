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

target_include_directories(z80e_objects SYSTEM PRIVATE
    include/
)

target_include_directories(z80e_objects PRIVATE
    include/z80e
)
