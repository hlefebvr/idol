if (NOT DEFINED COIN_OR_DIR)
    set(COIN_OR_DIR $ENV{COIN_OR_HOME})
endif()

find_path(
        ALPS_INCLUDE_DIRS
        NAMES Alps.h
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES include/coin)

find_library(
        ALPS_LIBRARY
        NAMES Alps
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ALPS DEFAULT_MSG
        ALPS_INCLUDE_DIRS
        ALPS_LIBRARY
)

if (ALPS_FOUND)
    add_library(alps SHARED IMPORTED)
    set_target_properties(alps PROPERTIES IMPORTED_LOCATION ${ALPS_LIBRARY})
    target_include_directories(alps INTERFACE ${ALPS_INCLUDE_DIRS})
endif()
