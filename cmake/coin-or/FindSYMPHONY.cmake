if (NOT DEFINED COIN_OR_DIR)
    set(COIN_OR_DIR $ENV{COIN_OR_HOME})
endif()

find_path(
        SYMPHONY_INCLUDE_DIRS
        NAMES symphony.h
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES include/coin)

find_library(
        SYMPHONY_LIBRARY
        NAMES Sym
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SYMPHONY DEFAULT_MSG
        SYMPHONY_INCLUDE_DIRS
        SYMPHONY_LIBRARY
)

if (SYMPHONY_FOUND)
    add_library(symphony SHARED IMPORTED)
    set_target_properties(symphony PROPERTIES IMPORTED_LOCATION ${SYMPHONY_LIBRARY})
    target_include_directories(symphony INTERFACE ${SYMPHONY_INCLUDE_DIRS})
endif()
