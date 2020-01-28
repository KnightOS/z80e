list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake)
find_package(Scas REQUIRED)

add_executable(tests
	test.c
	$<TARGET_OBJECTS:z80e_objects>
)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)

if(NOT EMSCRIPTEN AND NOT HAIKU AND NOT APPLE)
	target_link_libraries(tests rt)
endif()

TARGET_LINK_LIBRARIES(tests ${SCAS_LIBRARIES})
