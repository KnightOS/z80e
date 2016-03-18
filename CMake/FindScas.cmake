# - Find scas
# Finds scas includes and library
#
# SCAS_INCLUDES      - Headers
# SCAS_LIBRARIES     - Libraries
# SCAS_FOUND         - True if found

if(SCAS_INCLUDES)
  set (SCAS_FIND_QUIETLY TRUE)
endif()

find_path(SCAS_INCLUDES 8xp.h PATH_SUFFIXES scas)

find_library(SCAS_LIBRARIES NAMES scas libscas)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SCAS DEFAULT_MSG SCAS_LIBRARIES SCAS_INCLUDES)

if (SCAS_FOUND)
    message (STATUS "Found components for Libticables")
    message (STATUS "SCAS_INCLUDES = ${SCAS_INCLUDES}")
    message (STATUS "SCAS_LIBRARIES = ${SCAS_LIBRARIES}")
else (SCAS_FOUND)
    if (SCAS_FIND_REQUIRED)
        message (FATAL_ERROR "Could not find scas!")
    endif (SCAS_FIND_REQUIRED)
endif (SCAS_FOUND)

mark_as_advanced(SCAS_LIBRARIES SCAS_INCLUDES)
