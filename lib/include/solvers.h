//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_SOLVERS_H
#define OPTIMIZE_SOLVERS_H

#include "solvers/Types.h"
#include <tuple>

template<typename ... input_t> using tuple_cat_t= decltype(std::tuple_cat( std::declval<input_t>()... ));

#ifdef USE_GUROBI
#include "solvers/gurobi/Gurobi.h"
using gurobi_solver = std::tuple<Gurobi>;
#else
using gurobi_solver = std::tuple<>;
#endif

#ifdef USE_LPSOLVE
#include "solvers/lpsolve/Lpsolve.h"
using lpsolve_solver = std::tuple<Lpsolve>;
#else
using lpsolve_solver = std::tuple<>;
#endif

using available_solvers = tuple_cat_t<gurobi_solver, lpsolve_solver>;

#endif //OPTIMIZE_SOLVERS_H
