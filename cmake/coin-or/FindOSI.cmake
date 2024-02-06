find_path(
        OSI_INCLUDE_DIRS
        NAMES OsiConfig.h
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES include/coin)

if (${USE_CBC})
    find_library(
            OSI_CBC_LIBRARY
            NAMES OsiCbc
            HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
            PATH_SUFFIXES lib)
    list(APPEND REQUIRED_FILES "OSI_CBC_LIBRARY")
endif()

if (${USE_CLP})
    find_library(
            OSI_CLP_LIBRARY
            NAMES OsiClp
            HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
            PATH_SUFFIXES lib)
    list(APPEND REQUIRED_FILES "OSI_CLP_LIBRARY")
endif()

if (${USE_CPLEX})
    find_library(
            OSI_CPX_LIBRARY
            NAMES OsiCpx
            HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
            PATH_SUFFIXES lib)
    list(APPEND REQUIRED_FILES "OSI_CPX_LIBRARY")
endif()

if (${USE_SYMPHONY})
    find_library(
            OSI_SYM_LIBRARY
            NAMES OsiSym
            HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
            PATH_SUFFIXES lib)
    list(APPEND REQUIRED_FILES "OSI_SYM_LIBRARY")
endif()

find_library(
        OSI_LIBRARY
        NAMES Osi
        HINTS ${COIN_OR_DIR} $ENV{COIN_OR_HOME}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OSI DEFAULT_MSG
        OSI_INCLUDE_DIRS
        OSI_LIBRARY
        ${REQUIRED_FILES}
)

if (OSI_FOUND)
    add_library(osi SHARED IMPORTED)
    set_target_properties(osi PROPERTIES IMPORTED_LOCATION ${OSI_LIBRARY})

    if (OSI_CPX_LIBRARY)
        target_link_libraries(osi INTERFACE ${OSI_CPX_LIBRARY})
    endif()

    if (OSI_CPL_LIBRARY)
        target_link_libraries(osi INTERFACE ${OSI_CPL_LIBRARY})
    endif()

    if (OSI_SYM_LIBRARY)
        target_link_libraries(osi INTERFACE ${OSI_SYM_LIBRARY})
    endif()

    if (OSI_CBC_LIBRARY)
        target_link_libraries(osi INTERFACE ${OSI_CBC_LIBRARY})
    endif()

    target_include_directories(osi INTERFACE ${OSI_INCLUDE_DIRS})
endif()
