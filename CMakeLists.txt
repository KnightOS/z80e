cmake_minimum_required(VERSION 2.8.5)
project(z80e C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
add_definitions("-Wall -O3 -g")

FILE(GLOB Hardware ti/hardware/*.c)
FILE(GLOB Commands debugger/commands/*.c)
IF(EMSCRIPTEN)
    add_definitions("-DEMSCRIPTEN -Wno-warn-absolute-paths")
    FILE(GLOB Shims shims/*.c)
ELSE()
    set(SpecificFilesMain main.c)
ENDIF()

add_executable(tests
    core/cpu.c
    core/registers.c
    debugger/debugger.c
    debugger/tui.c
    debugger/hooks.c
    disassembler/disassemble.c
    ti/asic.c
    ti/memory.c
    runloop.c
    ${Hardware}
    ${Commands}
    ${Shims}
    test.c
)

add_executable(z80e
    core/cpu.c
    core/registers.c
    debugger/debugger.c
    debugger/tui.c
    debugger/hooks.c
    disassembler/disassemble.c
    ti/asic.c
    ti/memory.c
    runloop.c
    ${Hardware}
    ${Commands}
    ${Shims}
    ${SpecificFilesMain}
)

add_executable(zex
    core/cpu.c
    core/registers.c
    debugger/debugger.c
    debugger/tui.c
    debugger/hooks.c
    disassembler/disassemble.c
    ti/asic.c
    ti/memory.c
    runloop.c
    ${Hardware}
    ${Commands}
    ${Shims}
    tools/zex.c
)

IF(EMSCRIPTEN)
    SET_TARGET_PROPERTIES(z80e PROPERTIES LINK_FLAGS @${CMAKE_CURRENT_SOURCE_DIR}/emscripten.config)
ELSE()
    TARGET_LINK_LIBRARIES(z80e readline rt ncurses)
    SET_TARGET_PROPERTIES(z80e PROPERTIES COMPILE_FLAGS -DCURSES)
    TARGET_LINK_LIBRARIES(tests readline rt)
    TARGET_LINK_LIBRARIES(zex readline rt)
ENDIF()

include_directories(
    core/
    debugger/
    ti/
    ti/hardware/
    disassembler/
    shims/
    .
)

set(CMAKE_BUILD_TYPE Release)
