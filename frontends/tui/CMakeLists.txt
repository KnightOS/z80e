cmake_minimum_required(VERSION 2.6)
project (tui)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

add_executable(tui
    main.c
    tui.c
    $<TARGET_OBJECTS:z80e_objects>
)

set_target_properties(tui PROPERTIES APPEND_STRING PROPERTY
    COMPILE_FLAGS " ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX} ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include"
)

TARGET_LINK_LIBRARIES(tui readline)
