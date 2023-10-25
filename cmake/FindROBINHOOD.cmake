find_path(
        ROBINHOOD_INCLUDE_DIRS
        NAMES robin_hood.h
        HINTS /usr/include /usr/include/robin_hood/ /usr/local/include/ /usr/local/include/robin_hood/ ${ROBINHOOD_DIR} $ENV{ROBINHOOD_HOME}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ROBINHOOD DEFAULT_MSG ROBINHOOD_INCLUDE_DIRS)

if (ROBINHOOD_FOUND)
    add_library(robin_hood INTERFACE IMPORTED)
    target_include_directories(robin_hood INTERFACE ${ROBINHOOD_INCLUDE_DIRS})
endif()
