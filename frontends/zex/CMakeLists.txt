cmake_minimum_required(VERSION 2.8)
project (zex)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake)
find_package(Scas REQUIRED)

add_executable(zex
    zex.c
    $<TARGET_OBJECTS:z80e_objects>
)

TARGET_LINK_LIBRARIES(zex readline ${SCAS_LIBRARIES})

if(NOT APPLE AND NOT HAIKU)
	TARGET_LINK_LIBRARIES(zex rt)
endif()
