find_path(
        CGL_INCLUDE_DIRS
        NAMES CglConfig.h
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES include/coin)

find_library(
        CGL_LIBRARY
        NAMES Cgl
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CGL DEFAULT_MSG
        CGL_INCLUDE_DIRS
        CGL_LIBRARY
)

if (CGL_FOUND)
    add_library(cgl SHARED IMPORTED)
    set_target_properties(cgl PROPERTIES IMPORTED_LOCATION ${CGL_LIBRARY})
    target_include_directories(cgl INTERFACE ${CGL_INCLUDE_DIRS})
endif()
