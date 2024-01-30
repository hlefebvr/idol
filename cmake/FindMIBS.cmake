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
        COIN_UTILS_LIBRARY
        NAMES CoinUtils
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        OSI_LIBRARY
        NAMES Osi
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        ALPS_LIBRARY
        NAMES Alps
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        OSI_CLP_LIBRARY
        NAMES OsiClp
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

find_library(
        OSI_SYM_LIBRARY
        NAMES OsiSym
        HINTS ${MIBS_DIR} $ENV{MIBS_HOME}
        PATH_SUFFIXES lib)

#### END DEPENDENCIES

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MIBS DEFAULT_MSG MIBS_LIBRARY ALPS_LIBRARY OSI_LIBRARY OSI_CLP_LIBRARY OSI_SYM_LIBRARY COIN_UTILS_LIBRARY MIBS_INCLUDE_DIRS)

if (MIBS_FOUND)
    add_library(mibs STATIC IMPORTED)
    set_target_properties(mibs PROPERTIES IMPORTED_LOCATION ${MIBS_LIBRARY})
    target_link_libraries(mibs INTERFACE ${OSI_LIBRARY})
    target_link_libraries(mibs INTERFACE ${OSI_CLP_LIBRARY})
    target_link_libraries(mibs INTERFACE ${OSI_SYM_LIBRARY})
    target_link_libraries(mibs INTERFACE ${COIN_UTILS_LIBRARY})
    target_link_libraries(mibs INTERFACE ${ALPS_LIBRARY})
    target_include_directories(mibs INTERFACE ${MIBS_INCLUDE_DIRS})
endif()
