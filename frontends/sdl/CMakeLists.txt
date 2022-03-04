list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake)
find_package(SDL2 REQUIRED)
find_package(Scas REQUIRED)

add_executable(z80e-sdl
	main.c
	tui.c
	$<TARGET_OBJECTS:z80e_objects>
)

target_include_directories(z80e-sdl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)
TARGET_LINK_LIBRARIES(z80e-sdl readline SDL2 ${SCAS_LIBRARIES})

if(NOT APPLE AND NOT HAIKU)
	TARGET_LINK_LIBRARIES(z80e-sdl rt)
endif()

INSTALL(TARGETS z80e-sdl DESTINATION bin)
