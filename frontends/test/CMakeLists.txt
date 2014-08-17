cmake_minimum_required(VERSION 2.6)
project (z80e_objects)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

add_executable(tests
    test.c
    $<TARGET_OBJECTS:z80e_objects>
)
target_include_directories(tests SYSTEM PRIVATE
    ../../libz80e/include
)
