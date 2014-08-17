cmake_minimum_required(VERSION 2.6)
project (tui)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

add_executable(tui
    main.c
    tui.c
    $<TARGET_OBJECTS:z80e_objects>
)
target_include_directories(tui SYSTEM PRIVATE
    ../../libz80e/include
)

TARGET_LINK_LIBRARIES(tui readline)
