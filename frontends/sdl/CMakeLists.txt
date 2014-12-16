cmake_minimum_required(VERSION 2.8)
project (sdl)

find_package(SDL REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)

add_executable(z80e-sdl
	main.c
	tui.c
	$<TARGET_OBJECTS:z80e_objects>
)

if(APPLE)
	TARGET_LINK_LIBRARIES(z80e-sdl readline -lSDLmain ${SDL_LIBRARY})
else()
	TARGET_LINK_LIBRARIES(z80e-sdl readline rt ${SDL_LIBRARY})
endif()

if(WIN32)
	INSTALL(TARGETS z80e-sdl.exe
		RUNTIME DESTINATION bin)
else()
	INSTALL(TARGETS z80e-sdl
		RUNTIME DESTINATION bin)
endif()
