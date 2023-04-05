find_path(
        KP_SOLVER_INCLUDE_DIRS
        NAMES dynamic_programming_bellman.hpp
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES knapsacksolver/algorithms
)

find_library(
        KP_SOLVER_LIBRARY
        NAMES knapsacksolver
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES bazel-bin/knapsacksolver
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KPSOLVER DEFAULT_MSG KP_SOLVER_LIBRARY KP_SOLVER_INCLUDE_DIRS)

if (KPSOLVER_FOUND)
    add_library(knapsacksolver STATIC IMPORTED)
    set_target_properties(knapsacksolver PROPERTIES IMPORTED_LOCATION ${KP_SOLVER_LIBRARY})
    target_include_directories(knapsacksolver INTERFACE ${KP_SOLVER_INCLUDE_DIRS})
endif()