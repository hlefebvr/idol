find_path(
        MIBS_INCLUDE_DIRS
        NAMES MibSModel.hpp
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES include/coin)


find_library(
        MIBS_LIBRARY
        NAMES Mibs
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES lib)

find_library(
        MIBS_LIBRARY
        NAMES MibS
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MIBS DEFAULT_MSG
        MIBS_INCLUDE_DIRS
        MIBS_LIBRARY
)

if (MIBS_FOUND)
    add_library(mibs SHARED IMPORTED)
    set_target_properties(mibs PROPERTIES IMPORTED_LOCATION ${MIBS_LIBRARY})
    target_include_directories(mibs INTERFACE ${MIBS_INCLUDE_DIRS})
endif()
