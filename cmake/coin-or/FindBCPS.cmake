if (NOT DEFINED COIN_OR_DIR)
    set(COIN_OR_DIR $ENV{COIN_OR_HOME})
endif()

find_path(
        BCPS_INCLUDE_DIRS
        NAMES Bcps.h
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES include/coin)

find_library(
        BCPS_LIBRARY
        NAMES Bcps
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BCPS DEFAULT_MSG
        BCPS_INCLUDE_DIRS
        BCPS_LIBRARY
)

if (BCPS_FOUND)
    add_library(bcps SHARED IMPORTED)
    set_target_properties(bcps PROPERTIES IMPORTED_LOCATION ${BCPS_LIBRARY})
    target_include_directories(bcps INTERFACE ${BCPS_INCLUDE_DIRS})
endif()
