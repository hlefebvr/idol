//
// Created by henri on 02/09/22.
//

#ifndef OPTIMIZE_TEST_UTILS_H
#define OPTIMIZE_TEST_UTILS_H

#include "modeling.h"

#include "backends/solvers/Gurobi.h"
#include "backends/BranchAndBoundMIP.h"
#include "backends/parameters/Parameters_Algorithm.h"

#include "./cartesian_product.h"

#include <catch2/catch_all.hpp>

using namespace Catch::literals;

using lp_solvers = std::tuple<Gurobi>;
using milp_solvers = std::tuple<Gurobi, BranchAndBoundMIP<Gurobi>>;

#endif //OPTIMIZE_TEST_UTILS_H
