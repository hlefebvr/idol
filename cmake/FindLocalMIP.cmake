# Local-MIP
find_path(
        LOCAL_MIP_INCLUDE_DIRS
        NAMES model_api/Model_API.h
        HINTS ${LOCAL_MIP_DIR}
        PATH_SUFFIXES src)

find_library(
        LOCAL_MIP_LIBRARY
        NAMES LocalMIP
        HINTS ${LOCAL_MIP_DIR}
        PATH_SUFFIXES build
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LocalMIP DEFAULT_MSG
        LOCAL_MIP_INCLUDE_DIRS
        LOCAL_MIP_LIBRARY
)

if (LocalMIP_FOUND)
    add_library(local_mip SHARED IMPORTED)
    set_target_properties(local_mip PROPERTIES IMPORTED_LOCATION ${LOCAL_MIP_LIBRARY})
    target_include_directories(local_mip INTERFACE ${LOCAL_MIP_INCLUDE_DIRS})
endif()
