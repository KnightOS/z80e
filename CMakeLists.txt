cmake_minimum_required(VERSION 2.8.5)
project(z80e C)
set(CMAKE_C_FLAGS "-g -DENABLE_HOOKS -DENABLE_READ_BYTE_HOOK=1 -DENABLE_WRITE_BYTE_HOOK=1")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
add_definitions("-Wall")

FILE(GLOB Hardware ti/hardware/*.c)
FILE(GLOB Commands debugger/commands/*.c)
IF(EMSCRIPTEN)
    add_definitions("-DEMSCRIPTEN -Wno-warn-absolute-paths")
    FILE(GLOB Shims shims/*.c)
    set(ExecutableSuffix .bc)
ELSE()
    set(SpecificFilesMain main.c)
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
    runloop.c
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
    runloop.c
    ${Hardware}
    ${Commands}
    ${Shims}
    ${SpecificFilesMain}
)

add_executable(zex${ExecutableSuffix}
    core/cpu.c
    core/registers.c
    debugger/debugger.c
    debugger/hooks.c
    debugger/tui.c
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
    add_custom_command(
        TARGET z80e.bc
        POST_BUILD
        COMMAND ${CMAKE_C_COMPILER} -o z80e.js z80e.bc @${CMAKE_CURRENT_SOURCE_DIR}/emscripten.config
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
        COMMENT "Finalizing Emscripten build..."
        VERBATIM
    )

    add_custom_command(
        TARGET tests.bc
        POST_BUILD
        COMMAND ${CMAKE_C_COMPILER} -o tests.js tests.bc
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
        COMMENT "Finalizing Emscripten build..."
        VERBATIM
    )

    add_custom_command(
        TARGET zex.bc
        POST_BUILD
        COMMAND ${CMAKE_C_COMPILER} -o zex.js zex.bc
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
        COMMENT "Finalizing Emscripten build..."
        VERBATIM
    )
ELSE()
    TARGET_LINK_LIBRARIES(z80e${ExecutableSuffix} readline rt)
    TARGET_LINK_LIBRARIES(tests${ExecutableSuffix} readline rt)
    TARGET_LINK_LIBRARIES(zex${ExecutableSuffix} readline rt)
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
