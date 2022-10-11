//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_SOLVERS_H
#define OPTIMIZE_SOLVERS_H

#include "solvers/Types.h"
#include <tuple>

template<typename ... input_t> using tuple_cat_t= decltype(std::tuple_cat( std::declval<input_t>()... ));

#ifdef USE_GUROBI
#include "algorithms/external-solver/Solvers_Gurobi.h"

using gurobi_solver = std::tuple<Solvers::Gurobi>;
#else
using gurobi_solver = std::tuple<>;
#endif

#ifdef USE_GLPK
#include "algorithms/external-solver/Solvers_GLPK.h"

using glpk_simplex_solver = std::tuple<Solvers::GLPK_Simplex>;
#else
using glpk_solver = std::tuple<>;
#endif

using available_solvers = tuple_cat_t<gurobi_solver,
                                      glpk_simplex_solver
                                      >;

#endif //OPTIMIZE_SOLVERS_H
