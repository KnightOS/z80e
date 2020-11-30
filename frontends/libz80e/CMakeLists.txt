cmake_minimum_required(VERSION 2.8)
project (libz80e)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "bin/")

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include)

if(DEFINED EMSCRIPTEN)
add_executable(z80e
	$<TARGET_OBJECTS:z80e_objects>
	openti_helpers/Debugger/Debugger.c
) # bit of a hack to get it to compile to .js directly

SET_TARGET_PROPERTIES(z80e PROPERTIES LINK_FLAGS @${CMAKE_CURRENT_SOURCE_DIR}/emscripten.config)
else()
add_library(z80e SHARED
	$<TARGET_OBJECTS:z80e_objects>
)

if(NOT (APPLE OR HAIKU OR MINGW OR MSYS))
	TARGET_LINK_LIBRARIES(z80e rt)
endif()

endif()

TARGET_LINK_LIBRARIES(z80e ${SCAS_LIBRARIES})

INSTALL(TARGETS z80e
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../libz80e/include/z80e/
    DESTINATION include/z80e
)
