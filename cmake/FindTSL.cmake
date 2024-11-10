find_path(
        TSL_INCLUDE_DIRS
        NAMES sparse_map.h
        HINTS /usr/include /usr/include/robin_hood/ /usr/local/include/ /usr/local/include/tsl/ ${TSL_DIR} $ENV{TSL_HOME}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TSL DEFAULT_MSG TSL_INCLUDE_DIRS)

if (TSL_FOUND)
    add_library(tsl INTERFACE IMPORTED)
    target_include_directories(tsl INTERFACE ${TSL_INCLUDE_DIRS})
endif()
