//
// Created by henri on 02/09/22.
//

#ifndef OPTIMIZE_TEST_UTILS_H
#define OPTIMIZE_TEST_UTILS_H

#include "modeling.h"

#include "solvers.h"

#include <tuple>
#include "BackendNotAvailable.h"

template<typename ... input_t> using tuple_cat_t= decltype(std::tuple_cat( std::declval<input_t>()... ));

#ifdef IDOL_USE_GUROBI
#include "optimizers/solvers/Gurobi.h"

using gurobi_solver = std::tuple<GurobiOptimizer>;
#else
using gurobi_solver = std::tuple<>;
#endif

#ifdef IDOL_USE_GLPK
#include "optimizers/solvers/GLPK.h"

using glpk_solver = std::tuple<GLPKOptimizer>;
#else
using glpk_solver = std::tuple<>;
#endif

#ifdef IDOL_USE_MOSEK
#include "optimizers/solvers/Mosek.h"

using mosek_solver = std::tuple<MosekOptimizer>;
#else
using mosek_solver = std::tuple<>;
#endif

namespace impl {
    using lp_solvers = tuple_cat_t<gurobi_solver, glpk_solver, mosek_solver>;
    using milp_solvers = tuple_cat_t<gurobi_solver, glpk_solver, mosek_solver>;
}

constexpr bool has_lp_solver = std::tuple_size_v<impl::lp_solvers> > 0;
constexpr bool has_milp_solver = std::tuple_size_v<impl::milp_solvers> > 0;

using lp_solvers   = std::conditional_t< has_lp_solver,   impl::lp_solvers,   std::tuple<BackendNotAvailable>>;
using milp_solvers = std::conditional_t< has_milp_solver, impl::milp_solvers, std::tuple<BackendNotAvailable>>;

using default_solver = std::conditional_t<has_milp_solver, std::tuple_element_t<0, milp_solvers>,
        std::conditional_t<has_lp_solver, std::tuple_element_t<0, lp_solvers>, BackendNotAvailable>
>;


#include "optimizers/parameters/Parameters_Algorithm.h"

#include "./cartesian_product.h"

#include <catch2/catch_all.hpp>

using namespace Catch::literals;

#endif //OPTIMIZE_TEST_UTILS_H
