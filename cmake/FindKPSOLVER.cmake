# knapsacksolver

find_path(
        KP_SOLVER_INCLUDE_DIRS
        NAMES knapsacksolver
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES bazel-knapsacksolver
)

find_library(
        KP_SOLVER_LIBRARY
        NAMES knapsacksolver
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES bazel-bin/knapsacksolver
)

find_library(
        KP_SOLVER_DYNAMIC_PROGRAMMING_PRIMAL_DUAL_LIBRARY
        NAMES dynamic_programming_primal_dual
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES bazel-bin/knapsacksolver/algorithms
)

find_library(
        KP_SOLVER_GREEDY_LIBRARY
        NAMES greedy
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES bazel-bin/knapsacksolver/algorithms
)

find_library(
        KP_SOLVER_UPPER_BOUND_DANTZIG_LIBRARY
        NAMES upper_bound_dantzig
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES bazel-bin/knapsacksolver/algorithms
)

find_library(
        KP_SOLVER_SURROGATE_RELAXATION_LIBRARY
        NAMES surrogate_relaxation
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES bazel-bin/knapsacksolver/algorithms
)

# optimizationtools dependency

find_path(
        OPTIMIZATIONTOOLS_INCLUDE_DIRS
        NAMES optimizationtools
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES bazel-knapsacksolver/external/optimizationtools
)

find_library(
        OPTIMIZATIONTOOLS_LIBRARY
        NAMES info
        HINTS ${KP_SOLVER_DIR} $ENV{KP_SOLVER_HOME}
        PATH_SUFFIXES bazel-bin/external/optimizationtools/optimizationtools/utils
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KPSOLVER DEFAULT_MSG
        KP_SOLVER_LIBRARY
        KP_SOLVER_INCLUDE_DIRS
        OPTIMIZATIONTOOLS_INCLUDE_DIRS
        OPTIMIZATIONTOOLS_LIBRARY
        KP_SOLVER_DYNAMIC_PROGRAMMING_PRIMAL_DUAL_LIBRARY
        KP_SOLVER_GREEDY_LIBRARY
        KP_SOLVER_SURROGATE_RELAXATION_LIBRARY
        KP_SOLVER_UPPER_BOUND_DANTZIG_LIBRARY
    )

if (KPSOLVER_FOUND)
    add_library(knapsacksolver STATIC IMPORTED)
    set_target_properties(knapsacksolver PROPERTIES IMPORTED_LOCATION ${KP_SOLVER_LIBRARY})

    target_link_libraries(knapsacksolver INTERFACE ${KP_SOLVER_DYNAMIC_PROGRAMMING_PRIMAL_DUAL_LIBRARY})
    target_link_libraries(knapsacksolver INTERFACE ${OPTIMIZATIONTOOLS_LIBRARY})
    target_link_libraries(knapsacksolver INTERFACE ${KP_SOLVER_GREEDY_LIBRARY})
    target_link_libraries(knapsacksolver INTERFACE ${KP_SOLVER_SURROGATE_RELAXATION_LIBRARY})
    target_link_libraries(knapsacksolver INTERFACE ${KP_SOLVER_UPPER_BOUND_DANTZIG_LIBRARY})

    target_include_directories(knapsacksolver INTERFACE ${KP_SOLVER_INCLUDE_DIRS})
    target_include_directories(knapsacksolver INTERFACE ${KP_SOLVER_OPTIMIZATIONTOOLS_INCLUDE_DIRS})
endif()