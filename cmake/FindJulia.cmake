# Try to find the header file
find_path(Julia_INCLUDE_DIR
        NAMES julia.h
        PATHS "${JULIA_DIR}/include/julia"
        NO_DEFAULT_PATH
)

# Try to find the libjulia shared library
find_library(Julia_LIBRARIES
        NAMES julia
        PATHS "${JULIA_DIR}/lib"
        NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Julia REQUIRED_VARS Julia_INCLUDE_DIR Julia_LIBRARIES)

if (Julia_FOUND)
    add_library(julia STATIC IMPORTED)
    set_target_properties(julia PROPERTIES IMPORTED_LOCATION ${Julia_LIBRARIES})
    set_target_properties(julia PROPERTIES INTERFACE_LINK_OPTIONS "-Wl,-rpath,${JULIA_DIR}/lib")
    target_include_directories(julia INTERFACE ${Julia_INCLUDE_DIR})
endif()
