if (NOT DEFINED COIN_OR_DIR)
    set(COIN_OR_DIR $ENV{COIN_OR_HOME})
endif()

find_path(
        COINUTILS_INCLUDE_DIRS
        NAMES CoinModel.hpp
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES include/coin)

find_library(
        COINUTILS_LIBRARY
        NAMES CoinUtils
        HINTS ${COIN_OR_DIR}
        PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(COINUTILS DEFAULT_MSG
        COINUTILS_INCLUDE_DIRS
        COINUTILS_LIBRARY
)

if (COINUTILS_FOUND)
    add_library(coinUtils SHARED IMPORTED)
    set_target_properties(coinUtils PROPERTIES IMPORTED_LOCATION ${COINUTILS_LIBRARY})
    target_include_directories(coinUtils INTERFACE ${COINUTILS_INCLUDE_DIRS})
endif()
