# Building and solving simple models
add_example(lp-glpk)
add_example(kp-highs)
add_example(sos1-gurobi)
add_example(sudoku-cplex)

# More advanced features
add_example(lp-dual)
add_example(tsp-lazy)

# Branch-and-bound algorithm
add_example(flp-bab)

# Branch-and-price algorithm
add_example(gap-bap)
add_example(bin-bap)

find_package(Boost 1.80 QUIET CONFIG COMPONENTS graph)

if(NOT Boost_FOUND)
    message(STATUS "Boost not found on system. Downloading by FetchContent.")
    FetchContent_Declare(Boost URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz)
    set(BOOST_INCLUDE_LIBRARIES graph CACHE INTERNAL "")
    FetchContent_MakeAvailable(Boost)
endif()
add_example(lambda-optimizer)
target_link_libraries(example-lambda-optimizer PRIVATE Boost::graph)

# Legacy examples
#add_example(knapsack-cover)
#add_example(assignment-penalty-bap)
#add_example(benders)
