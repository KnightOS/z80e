cmake_minimum_required(VERSION 2.8.5)
project(z80e C)
set(CMAKE_C_FLAGS "-g -DENABLE_HOOKS -DENABLE_READ_BYTE_HOOK=1 -DENABLE_WRITE_BYTE_HOOK=1")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
add_definitions("-Wall")

FILE(GLOB Hardware ti/hardware/*.c)
FILE(GLOB Commands debugger/commands/*.c)
IF(EMSCRIPTEN)
    add_definitions("-DEMSCRIPTEN")
    FILE(GLOB Shims shims/*.c)
ELSE()
ENDIF()

add_executable(tests
    core/cpu.c
    core/registers.c
    debugger/debugger.c
    debugger/hooks.c
    debugger/tui.c
    disassembler/disassemble.c
    ti/asic.c
    ti/memory.c
    ${Hardware}
    ${Commands}
    ${Shims}
    test.c
)

add_executable(z80e
    core/cpu.c
    core/registers.c
    debugger/debugger.c
    debugger/hooks.c
    debugger/tui.c
    disassembler/disassemble.c
    ti/asic.c
    ti/memory.c
    ${Hardware}
    ${Commands}
    ${Shims}
    main.c
)

IF(NOT EMSCRIPTEN)
    TARGET_LINK_LIBRARIES(z80e readline)
    TARGET_LINK_LIBRARIES(tests readline)
ENDIF()

include_directories(
    core/
    debugger/
    ti/
    ti/hardware/
    disassembler/
    shims/
)

set(CMAKE_BUILD_TYPE Release)
