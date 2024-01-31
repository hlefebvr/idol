find_path(
        MIBS_INCLUDE_DIRS
        NAMES MibSModel.hpp
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES include/coin)


find_library(
        MIBS_LIBRARY
        NAMES Mibs
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

#### BEGIN DEPENDENCIES

find_library(
        ALPS_LIBRARY
        NAMES Alps
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        BCPS_LIBRARY
        NAMES Bcps
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        BLIS_LIBRARY
        NAMES Blis
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        CBC_LIBRARY
        NAMES Cbc
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        CBC_SOLVER_LIBRARY
        NAMES CbcSolver
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        CGL_LIBRARY
        NAMES Cgl
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        CLP_LIBRARY
        NAMES Clp
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        CLP_SOLVER_LIBRARY
        NAMES ClpSolver
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        COIN_UTILS_LIBRARY
        NAMES CoinUtils
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        MIBS_LIBRARY
        NAMES MibS
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        OSI_CBC_LIBRARY
        NAMES OsiCbc
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        OSI_CLP_LIBRARY
        NAMES OsiClp
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        OSI_CPX_LIBRARY
        NAMES OsiCpx
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        OSI_LIBRARY
        NAMES Osi
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        OSI_SYM_LIBRARY
        NAMES OsiSym
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        SYM_SOLVER_LIBRARY
        NAMES Sym
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        CPLEX_LIBRARY
        NAMES cplex
        HINTS /opt/ibm/ILOG/CPLEX_Studio2211/cplex/
        PATH_SUFFIXES /lib/x86-64_linux/static_pic/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MIBS DEFAULT_MSG
        ALPS_LIBRARY
        BCPS_LIBRARY
        BLIS_LIBRARY
        CBC_SOLVER_LIBRARY
        CGL_LIBRARY
        CLP_LIBRARY
        CLP_SOLVER_LIBRARY
        COIN_UTILS_LIBRARY
        OSI_CBC_LIBRARY
        OSI_CLP_LIBRARY
        OSI_CPX_LIBRARY
        OSI_LIBRARY
        OSI_SYM_LIBRARY
        SYM_SOLVER_LIBRARY
        CPLEX_LIBRARY
)

#find_package(ZLIB REQUIRED)
#find_package(LAPACK REQUIRED)

if (MIBS_FOUND)

    add_library(mibs SHARED IMPORTED)
    set_target_properties(mibs PROPERTIES IMPORTED_LOCATION ${MIBS_LIBRARY})
    target_link_libraries(mibs INTERFACE ${ALPS_LIBRARY})
    target_link_libraries(mibs INTERFACE ${BCPS_LIBRARY})
    target_link_libraries(mibs INTERFACE ${BLIS_LIBRARY})
    target_link_libraries(mibs INTERFACE ${CBC_SOLVER_LIBRARY})
    target_link_libraries(mibs INTERFACE ${CGL_LIBRARY})
    target_link_libraries(mibs INTERFACE ${CLP_LIBRARY})
    target_link_libraries(mibs INTERFACE ${CLP_SOLVER_LIBRARY})
    target_link_libraries(mibs INTERFACE ${COIN_UTILS_LIBRARY})
    target_link_libraries(mibs INTERFACE ${OSI_CBC_LIBRARY})
    target_link_libraries(mibs INTERFACE ${OSI_CLP_LIBRARY})
    target_link_libraries(mibs INTERFACE ${OSI_CPX_LIBRARY})
    target_link_libraries(mibs INTERFACE ${OSI_LIBRARY})
    target_link_libraries(mibs INTERFACE ${OSI_SYM_LIBRARY})
    target_link_libraries(mibs INTERFACE ${SYM_SOLVER_LIBRARY})
    target_link_libraries(mibs INTERFACE ${CPLEX_LIBRARY})
    #target_link_libraries(mibs INTERFACE LAPACK::LAPACK)
    target_include_directories(mibs INTERFACE ${MIBS_INCLUDE_DIRS})
endif()
