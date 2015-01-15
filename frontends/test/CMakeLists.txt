cmake_minimum_required(VERSION 2.8)
project (tests)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)

add_executable(tests
	test.c
	$<TARGET_OBJECTS:z80e_objects>
)

if(NOT EMSCRIPTEN AND NOT HAIKU AND NOT APPLE)
	target_link_libraries(tests rt)
endif()
