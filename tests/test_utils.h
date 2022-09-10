//
// Created by henri on 02/09/22.
//

#ifndef OPTIMIZE_TEST_UTILS_H
#define OPTIMIZE_TEST_UTILS_H

#include "modeling.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "solvers/gurobi/Gurobi.h"

#include <catch2/catch_all.hpp>

using namespace Catch::literals;

template<typename ... input_t> using tuple_cat_t= decltype(std::tuple_cat( std::declval<input_t>()... ));

#ifdef USE_GUROBI
using gurobi_solver = std::tuple<Gurobi>;
#else
using gurobi_solver = std::tuple<>;
#endif

#ifdef USE_LPSOLVE
using lpsolve_solver = std::tuple<Lpsolve>;
#else
using lpsolve_solver = std::tuple<>;
#endif

using for_each_solver = tuple_cat_t<lpsolve_solver, gurobi_solver>;

#endif //OPTIMIZE_TEST_UTILS_H
