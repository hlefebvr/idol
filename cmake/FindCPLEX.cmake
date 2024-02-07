if (NOT DEFINED CPLEX_DIR)
    set(CPLEX_DIR $ENV{CPLEX_HOME})
endif()

find_path(
        CPLEX_INCLUDE_DIRS
        NAMES cplex.h
        HINTS /opt/ibm/ILOG/CPLEX_Studio2211/cplex/ ${CPLEX_DIR}
        PATH_SUFFIXES include/ilcplex)

find_library(
        CPLEX_LIBRARY
        NAMES cplex
        HINTS /opt/ibm/ILOG/CPLEX_Studio2211/cplex/ ${CPLEX_DIR}
        PATH_SUFFIXES /lib/x86-64_linux/static_pic/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CPLEX DEFAULT_MSG
        CPLEX_INCLUDE_DIRS
        CPLEX_LIBRARY
)

if (CPLEX_FOUND)
    add_library(cplex SHARED IMPORTED)
    set_target_properties(cplex PROPERTIES IMPORTED_LOCATION ${CPLEX_LIBRARY})
    target_include_directories(cplex INTERFACE ${CPLEX_INCLUDE_DIRS})
endif()
