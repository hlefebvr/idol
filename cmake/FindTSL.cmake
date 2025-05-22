find_path(
        TSL_SPARSE_MAP_INCLUDE_DIRS
        NAMES sparse_map.h
        HINTS /usr/include /usr/local/include/ /usr/local/include/tsl/ ${TSL_DIR} $ENV{TSL_HOME} ${TSL_SPARSE_MAP_DIR} $ENV{TSL_SPARSE_MAP_HOME}
)

find_path(
        TSL_ORDERED_MAP_INCLUDE_DIRS
        NAMES ordered_map.h
        HINTS /usr/include /usr/local/include/ /usr/local/include/tsl/ ${TSL_ORDERED_DIR} $ENV{TSL_ORDERED_HOME}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TSL DEFAULT_MSG TSL_SPARSE_MAP_INCLUDE_DIRS TSL_ORDERED_MAP_INCLUDE_DIRS)

if (TSL_FOUND)
    add_library(tsl INTERFACE IMPORTED)
    target_include_directories(tsl INTERFACE ${TSL_SPARSE_MAP_INCLUDE_DIRS})
    target_include_directories(tsl INTERFACE ${TSL_ORDERED_MAP_INCLUDE_DIRS})
endif()
