//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_SOLVERS_H
#define OPTIMIZE_SOLVERS_H

#include "solvers/Types.h"
#include <tuple>

template<typename ... input_t> using tuple_cat_t= decltype(std::tuple_cat( std::declval<input_t>()... ));

#ifdef IDOL_USE_GUROBI
#include "algorithms/solvers/Solvers_Gurobi.h"

using gurobi_solver = std::tuple<Solvers::Gurobi>;
#else
using gurobi_solver = std::tuple<>;
#endif

#ifdef IDOL_USE_GLPK
#include "algorithms/solvers/Solvers_GLPK_Simplex.h"

using glpk_simplex_solver = std::tuple<Solvers::GLPK_Simplex>;
#else
using glpk_simplex_solver = std::tuple<>;
#endif

namespace impl {
    using lp_solvers = tuple_cat_t<gurobi_solver, glpk_simplex_solver>;
    using milp_solvers = tuple_cat_t<gurobi_solver>;
}

constexpr bool has_lp_solver = std::tuple_size_v<impl::lp_solvers> > 0;
constexpr bool has_milp_solver = std::tuple_size_v<impl::milp_solvers> > 0;

using lp_solvers   = std::conditional_t< has_lp_solver,   impl::lp_solvers,   std::tuple<VoidAlgorithm>>;
using milp_solvers = std::conditional_t< has_milp_solver, impl::milp_solvers, std::tuple<VoidAlgorithm>>;

using default_solver = std::conditional_t<has_milp_solver, std::tuple_element_t<0, milp_solvers>,
                        std::conditional_t<has_lp_solver, std::tuple_element_t<0, lp_solvers>, VoidAlgorithm>
                       >;

#endif //OPTIMIZE_SOLVERS_H
