if (NOT DEFINED CPLEX_DIR)
    set(CPLEX_DIR $ENV{CPLEX_HOME})
endif()

file(GLOB CPLEX_IN_OPT "/opt/ibm/ILOG/CPLEX_Studio*")

if(CPLEX_IN_OPT_LEN GREATER 0)
    list(GET CPLEX_IN_OPT 0 CPLEX_IN_OPT)
endif ()

# cplex
find_path(
        CPLEX_INCLUDE_DIRS
        NAMES ilcplex/cplex.h
        HINTS ${CPLEX_DIR}/cplex ${CPLEX_IN_OPT}/cplex/
        PATH_SUFFIXES include)

find_library(
        CPLEX_LIBRARY
        NAMES cplex
        HINTS ${CPLEX_DIR} ${CPLEX_IN_OPT}/cplex/
        PATH_SUFFIXES /lib/x86-64_linux/static_pic/
)

# concert
find_path(
        CPLEX_CONCERT_INCLUDE_DIRS
        NAMES ilconcert/iloenv.h
        HINTS ${CPLEX_DIR}/concert ${CPLEX_IN_OPT}/concert/
        PATH_SUFFIXES include)

find_library(
        CPLEX_CONCERT_LIBRARY
        NAMES concert
        HINTS ${CPLEX_DIR}/concert ${CPLEX_IN_OPT}/concert
        PATH_SUFFIXES /lib/x86-64_linux/static_pic
)

# ilocplex
find_library(
        CPLEX_ILOCPLEX_LIBRARY
        NAMES ilocplex
        HINTS ${CPLEX_DIR}/cplex ${CPLEX_IN_OPT}/cplex
        PATH_SUFFIXES /lib/x86-64_linux/static_pic
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CPLEX DEFAULT_MSG
        CPLEX_INCLUDE_DIRS
        CPLEX_LIBRARY
        CPLEX_CONCERT_INCLUDE_DIRS
        CPLEX_CONCERT_LIBRARY
        CPLEX_ILOCPLEX_LIBRARY
)

if (CPLEX_FOUND)
    add_library(cplex SHARED IMPORTED)
    set_target_properties(cplex PROPERTIES IMPORTED_LOCATION ${CPLEX_LIBRARY})
    target_link_libraries(cplex INTERFACE ${CPLEX_CONCERT_LIBRARY} ${CPLEX_ILOCPLEX_LIBRARY})
    target_include_directories(cplex INTERFACE ${CPLEX_INCLUDE_DIRS})
    target_include_directories(cplex INTERFACE ${CPLEX_CONCERT_INCLUDE_DIRS})
endif()
