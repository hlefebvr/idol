
find_path(
        LPSOLVE_INCLUDE_DIRS
        NAMES lp_lib.h
        HINTS /usr/include/ ${LPSOLVE_DIR} $ENV{LPSOLVE_HOME}
        PATH_SUFFIXES lpsolve
)

find_library(
        LPSOLVE_LIBRARY
        NAMES lpsolve55
        HINTS /usr/bin/ ${LPSOLVE_DIR} $ENV{LPSOLVE_HOME}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LPSOLVE DEFAULT_MSG LPSOLVE_LIBRARY LPSOLVE_INCLUDE_DIRS)

if (LPSOLVE_FOUND)
    add_library(lpsolve STATIC IMPORTED)
    set_target_properties(lpsolve PROPERTIES IMPORTED_LOCATION ${LPSOLVE_LIBRARY})
    target_include_directories(lpsolve INTERFACE ${LPSOLVE_INCLUDE_DIRS})
    target_link_libraries(lpsolve INTERFACE dl colamd)
endif()