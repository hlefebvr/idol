find_path(
        MOSEK_INCLUDE_DIRS
        NAMES fusion.h
        HINTS ${MOSEK_DIR} $ENV{MOSEK_HOME}
        PATH_SUFFIXES h
)

find_library(
        MOSEK_LIBRARY
        NAMES mosek64 mosek
        HINTS ${MOSEK_DIR} $ENV{MOSEK_HOME}
        PATH_SUFFIXES bin)

find_library(
        FUSION_LIBRARY
        NAMES fusion64 fusion
        HINTS ${MOSEK_DIR} $ENV{MOSEK_HOME}
        PATH_SUFFIXES bin)


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MOSEK DEFAULT_MSG MOSEK_LIBRARY FUSION_LIBRARY MOSEK_INCLUDE_DIRS)

if (MOSEK_FOUND)
    add_library(mosek STATIC IMPORTED)
    set_target_properties(mosek PROPERTIES IMPORTED_LOCATION ${FUSION_LIBRARY})
    target_link_libraries(mosek INTERFACE ${MOSEK_LIBRARY})
    target_include_directories(mosek INTERFACE ${MOSEK_INCLUDE_DIRS})
endif()
