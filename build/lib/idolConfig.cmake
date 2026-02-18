include(CMakeFindDependencyMacro)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/local/lib/cmake/idol/")

set(USE_GUROBI )
set(USE_GLPK )
set(USE_MOSEK OFF)
set(USE_EIGEN OFF)

find_dependency(OpenMP REQUIRED)

if (OFF)

    set(MOSEK_DIR "")
    find_dependency(MOSEK REQUIRED)
    add_compile_definitions(IDOL_USE_MOSEK)

endif()

if (OFF)

    set(EIGEN_DIR "")
    find_dependency(EIGEN REQUIRED)
    add_compile_definitions(IDOL_USE_EIGEN)

endif()

if (OFF)

    set(CPLEX_DIR "")
    find_dependency(CPLEX REQUIRED)
    add_compile_definitions(IDOL_USE_CPLEX)

endif()

if ()

    set(COIN_OR_DIR "~/Research/solvers/coin-or/dist")
    find_dependency(SYMPHONY REQUIRED)
    add_compile_definitions(IDOL_USE_SYMPHONY)

endif()

if ()

    set(COIN_OR_DIR "~/Research/solvers/coin-or/dist")
    find_dependency(CBC REQUIRED)
    add_compile_definitions(IDOL_USE_CBC)

endif()

if ()

    set(COIN_OR_DIR "~/Research/solvers/coin-or/dist")
    find_dependency(CLP REQUIRED)
    add_compile_definitions(IDOL_USE_CLP)

endif()

if (OFF)

    find_dependency(ROOT REQUIRED)
    add_compile_definitions(IDOL_USE_ROOT)

endif()

if (OFF)

    set(ROBINHOOD_DIR "")
    find_dependency(ROBINHOOD REQUIRED)
    add_compile_definitions(IDOL_USE_ROBINHOOD)

endif()

if (YES)

    set(COIN_OR_DIR "~/Research/solvers/coin-or/dist")

    find_dependency(ALPS REQUIRED)
    find_dependency(BCPS REQUIRED)
    find_dependency(BLIS REQUIRED)
    find_dependency(CGL REQUIRED)

    find_dependency(MIBS REQUIRED)

    add_compile_definitions(idol PUBLIC IDOL_USE_MIBS)

endif()

if (ON)

    set(COIN_OR_DIR "~/Research/solvers/coin-or/dist")
    find_dependency(COINUTILS REQUIRED)
    find_dependency(OSI REQUIRED)

    add_compile_definitions(IDOL_USE_OSI)

endif()

include("${CMAKE_CURRENT_LIST_DIR}/idolTargets.cmake")
