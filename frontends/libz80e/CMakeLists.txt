cmake_minimum_required(VERSION 2.6)
project (libz80e)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

add_library(z80e SHARED
    $<TARGET_OBJECTS:z80e_objects>
)

set_target_properties(z80e PROPERTIES APPEND_STRING PROPERTY
    COMPILE_FLAGS " ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX} ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include"
)
if(NOT DEFINED EMSCRIPTEN)
TARGET_LINK_LIBRARIES(z80e rt)
endif()
