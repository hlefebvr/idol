find_path(
        ROCPP_INCLUDE_DIRS
        NAMES ROCPP.h
        HINTS ${ROCPP_DIR}
        PATH_SUFFIXES src/include)

find_library(
        ROCPP_LIBRARY
        NAMES ROCPP
        HINTS ${ROCPP_DIR}
        PATH_SUFFIXES lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ROCPP DEFAULT_MSG
        ROCPP_INCLUDE_DIRS
        ROCPP_LIBRARY
)

if (ROCPP_FOUND)
    add_library(rocpp SHARED IMPORTED)
    set_target_properties(rocpp PROPERTIES IMPORTED_LOCATION ${ROCPP_LIBRARY})
    target_include_directories(rocpp INTERFACE ${ROCPP_INCLUDE_DIRS})
endif()
