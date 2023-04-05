
find_path(
        GLPK_INCLUDE_DIRS
        NAMES glpk.h
        HINTS /usr/include/ ${GLPK_DIR} $ENV{GLPK_HOME}
)

find_library(
        GLPK_LIBRARY
        NAMES glpk
        HINTS /usr/lib/ ${GLPK_DIR} $ENV{GLPK_HOME}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLPK DEFAULT_MSG GLPK_LIBRARY GLPK_INCLUDE_DIRS)

if (GLPK_FOUND)
    add_library(glpk STATIC IMPORTED)
    set_target_properties(glpk PROPERTIES IMPORTED_LOCATION ${GLPK_LIBRARY})
    target_include_directories(glpk INTERFACE ${GLPK_INCLUDE_DIRS})
endif()