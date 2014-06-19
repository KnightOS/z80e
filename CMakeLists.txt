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
    set(ExecutableSuffix .bc)
ELSE()
ENDIF()

add_executable(tests${ExecutableSuffix}
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

add_executable(z80e${ExecutableSuffix}
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
    ${SpecificFilesMain}
)

IF(NOT EMSCRIPTEN)
    TARGET_LINK_LIBRARIES(z80e${ExecutableSuffix} readline)
    TARGET_LINK_LIBRARIES(tests${ExecutableSuffix} readline)
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
