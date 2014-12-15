cmake_minimum_required(VERSION 2.8)
project (tui)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)

add_executable(z80e-tui
	main.c
	tui.c
	$<TARGET_OBJECTS:z80e_objects>
)

TARGET_LINK_LIBRARIES(z80e-tui readline)
if(NOT APPLE)
TARGET_LINK_LIBRARIES(z80e-tui readline rt)
endif()

if(WIN32)
	INSTALL(TARGETS z80e-tui.exe
		RUNTIME DESTINATION bin)
else()
	INSTALL(TARGETS z80e-tui
		RUNTIME DESTINATION bin)
endif()
