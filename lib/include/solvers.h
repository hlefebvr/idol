//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_SOLVERS_H
#define OPTIMIZE_SOLVERS_H

#include <tuple>
#include "backends/NoAvailableBackend.h"
#include "backends/BranchAndBoundMIP.h"
#include "backends/BranchAndPriceMIP.h"

template<typename ... input_t> using tuple_cat_t= decltype(std::tuple_cat( std::declval<input_t>()... ));

#ifdef IDOL_USE_GUROBI
#include "backends/solvers/Gurobi.h"

using gurobi_solver = std::tuple<Gurobi>;
using branch_and_bound_with_gurobi = std::tuple<BranchAndBoundMIP<Gurobi>>;
using branch_and_price_with_gurobi = std::tuple<BranchAndPriceMIP<Gurobi>>;
#else
using gurobi_solver = std::tuple<>;
using branch_and_bound_with_gurobi = std::tuple<>;
#endif

#ifdef IDOL_USE_GLPK
#include "backends/solvers/GLPK.h"

using glpk_solver = std::tuple<GLPK>;
using branch_and_bound_with_glpk = std::tuple<BranchAndBoundMIP<GLPK>>;
using branch_and_price_with_glpk = std::tuple<BranchAndPriceMIP<GLPK>>;
#else
using glpk_solver = std::tuple<>;
using branch_and_bound_with_glpk = std::tuple<>;
#endif

namespace impl {
    using lp_solvers = tuple_cat_t<gurobi_solver, glpk_solver>;
    using milp_solvers = tuple_cat_t<gurobi_solver, branch_and_bound_with_gurobi, glpk_solver, branch_and_bound_with_glpk>;
    using branch_and_price_solvers = tuple_cat_t<branch_and_price_with_gurobi, branch_and_price_with_glpk>;
}

constexpr bool has_lp_solver = std::tuple_size_v<impl::lp_solvers> > 0;
constexpr bool has_milp_solver = std::tuple_size_v<impl::milp_solvers> > 0;
constexpr bool has_branch_and_price_solver = std::tuple_size_v<impl::branch_and_price_solvers> > 0;

using lp_solvers   = std::conditional_t< has_lp_solver,   impl::lp_solvers,   std::tuple<NoAvailableBackend>>;
using milp_solvers = std::conditional_t< has_milp_solver, impl::milp_solvers, std::tuple<NoAvailableBackend>>;
using branch_and_price_solvers = std::conditional_t< has_branch_and_price_solver, impl::branch_and_price_solvers , std::tuple<NoAvailableBackend>>;

using default_solver = std::conditional_t<has_milp_solver, std::tuple_element_t<0, milp_solvers>,
                        std::conditional_t<has_lp_solver, std::tuple_element_t<0, lp_solvers>, NoAvailableBackend>
                       >;

#endif //OPTIMIZE_SOLVERS_H
