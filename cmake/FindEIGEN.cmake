if (NOT DEFINED EIGEN_DIR)
    set(EIGEN_DIR $ENV{EIGEN_HOME})
endif()

find_path(
        EIGEN_INCLUDE_DIRS
        NAMES Eigen/Core
        HINTS /usr/include/ /usr/local/include/ ${EIGEN_DIR} $ENV{EIGEN_HOME}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EIGEN DEFAULT_MSG EIGEN_INCLUDE_DIRS)

if (EIGEN_FOUND)
    add_library(eigen INTERFACE IMPORTED)
    target_include_directories(eigen INTERFACE ${EIGEN_INCLUDE_DIRS})
endif()
