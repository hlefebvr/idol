find_path(
        CBC_INCLUDE_DIRS
        NAMES CbcConfig.h
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES include/coin)

find_library(
        CBC_LIBRARY
        NAMES Cbc
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CBC DEFAULT_MSG
        CBC_INCLUDE_DIRS
        CBC_LIBRARY
)

if (CBC_FOUND)
    add_library(cbc SHARED IMPORTED)
    set_target_properties(cbc PROPERTIES IMPORTED_LOCATION ${CBC_LIBRARY})
    target_include_directories(cbc INTERFACE ${CBC_INCLUDE_DIRS})
endif()
