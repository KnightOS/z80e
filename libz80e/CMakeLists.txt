cmake_minimum_required(VERSION 3.10)
project (z80e_objects)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/../CMake)
message(STATUS ${CMAKE_MODULE_PATH})
find_package(Scas REQUIRED)
FILE(GLOB Hardware src/ti/hardware/*.c)
FILE(GLOB Commands src/debugger/commands/*.c)
FILE(GLOB scas_common ../scas/common/*.c)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/z80e)
include_directories(${SCAS_INCLUDES})

if(DEFINED EMSCRIPTEN)
add_compile_options(-Wno-warn-absolute-paths)
add_definitions(-DEMSCRIPTEN)
endif()

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
    ${scas_common}
)
