cmake_minimum_required(VERSION 2.8)
project (sdl)

find_package(SDL REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)

add_executable(sdl
	main.c
	tui.c
	$<TARGET_OBJECTS:z80e_objects>
)

TARGET_LINK_LIBRARIES(sdl readline rt ${SDL_LIBRARY})
