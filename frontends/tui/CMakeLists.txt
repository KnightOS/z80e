list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake)
find_package(Scas REQUIRED)

add_executable(z80e-tui
	main.c
	tui.c
	$<TARGET_OBJECTS:z80e_objects>
)

target_include_directories(z80e-tui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)
TARGET_LINK_LIBRARIES(z80e-tui readline)

if(NOT APPLE AND NOT HAIKU)
	TARGET_LINK_LIBRARIES(z80e-tui rt)
endif()
TARGET_LINK_LIBRARIES(z80e-tui ${SCAS_LIBRARIES})

INSTALL(TARGETS z80e-tui DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
