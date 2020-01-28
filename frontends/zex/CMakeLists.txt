list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake)
find_package(Scas REQUIRED)

add_executable(zex
    zex.c
    $<TARGET_OBJECTS:z80e_objects>
)

target_include_directories(zex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)
TARGET_LINK_LIBRARIES(zex readline ${SCAS_LIBRARIES})

if(NOT APPLE AND NOT HAIKU)
	TARGET_LINK_LIBRARIES(zex rt)
endif()
