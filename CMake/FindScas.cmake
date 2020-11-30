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

if (NOT SCAS_FOUND AND SCAS_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find scas!")
endif ()

mark_as_advanced(SCAS_LIBRARIES SCAS_INCLUDES)
