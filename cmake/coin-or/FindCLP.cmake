find_path(
        CLP_INCLUDE_DIRS
        NAMES ClpConfig.h
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES include/coin)

find_library(
        CLP_LIBRARY
        NAMES Clp
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES lib)

find_library(
        CLP_SOLVER_LIBRARY
        NAMES ClpSolver
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CLP DEFAULT_MSG
        CLP_INCLUDE_DIRS
        CLP_LIBRARY
        CLP_SOLVER_LIBRARY
)

if (CLP_FOUND)
    add_library(clp SHARED IMPORTED)
    set_target_properties(clp PROPERTIES IMPORTED_LOCATION ${CLP_LIBRARY})
    target_link_libraries(clp INTERFACE ${CLP_SOLVER_LIBRARY})
    target_include_directories(clp INTERFACE ${CLP_INCLUDE_DIRS})
endif()
