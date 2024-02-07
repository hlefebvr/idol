if (NOT DEFINED COIN_OR_DIR)
    set(COIN_OR_DIR $ENV{COIN_OR_HOME})
endif()

find_path(
        BLIS_INCLUDE_DIRS
        NAMES Blis.h
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES include/coin)

find_library(
        BLIS_LIBRARY
        NAMES Blis
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BLIS DEFAULT_MSG
        BLIS_INCLUDE_DIRS
        BLIS_LIBRARY
)

if (BLIS_FOUND)
    add_library(blis SHARED IMPORTED)
    set_target_properties(blis PROPERTIES IMPORTED_LOCATION ${BLIS_LIBRARY})
    target_include_directories(blis INTERFACE ${BLIS_INCLUDE_DIRS})
endif()
