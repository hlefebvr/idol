#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/Osi.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"

using namespace idol;

TEST_CASE("Basic bounded LP", "[basic][solver][lp]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., Inf, Continuous, 0., "x");
    const auto y = model.add_var(0., Inf, Continuous, 0., "y");
    model.add_ctr(x + y <= 4.);
    model.add_ctr(x <= 2.);
    model.add_ctr(y <= 3.);
    model.set_obj_expr(-3 * x - 2 * y);
    model.use(OPTIMIZER());
    model.optimize();

    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == Catch::Approx(-10.));
    CHECK(model.get_var_primal(x) == Catch::Approx(2.));
    CHECK(model.get_var_primal(y) == Catch::Approx(2.));
}

TEST_CASE("Basic binary MILP", "[basic][solver][binary-milp]") {
    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), 0., 1., Binary, 0., "x");
    model.add_ctr(x[0] + x[1] + 2 * x[2] <= 2.);
    model.set_obj_expr(-3 * x[0] - 2 * x[1] - 4 * x[2]);
    model.use(OPTIMIZER());
    model.optimize();

    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == Catch::Approx(-5.));
    CHECK(model.get_var_primal(x[0]) == Catch::Approx(1.));
    CHECK(model.get_var_primal(x[1]) == Catch::Approx(1.));
    CHECK(model.get_var_primal(x[2]) == Catch::Approx(0.));
}

TEST_CASE("Basic general-integer MILP", "[basic][solver][integer-milp]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0., 2., Integer, 0., "x");
    const auto y = model.add_var(0., 2., Continuous, 0., "y");
    model.add_ctr(2 * x + y <= 5.);
    model.set_obj_expr(-3 * x - y);
    model.use(OPTIMIZER());
    model.optimize();

    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == Catch::Approx(-7.));
    CHECK(model.get_var_primal(x) == Catch::Approx(2.));
    CHECK(model.get_var_primal(y) == Catch::Approx(1.));
}

TEST_CASE("Basic infeasible model", "[basic][solver][infeasible]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(-Inf, Inf, Continuous, 0., "x");
    model.add_ctr(x >= 1.);
    model.add_ctr(x <= 0.);
    model.use(OPTIMIZER());
    model.optimize();
    CHECK(model.get_status() == Infeasible);
}

TEST_CASE("Default node save uses original variable types for integrality infeasibility", "[basic][solver][branch-and-bound][node-info]") {
    Env env;
    Model original(env);
    const auto integer = original.add_var(0., 10., Integer, 0., "integer");
    const auto binary = original.add_var(0., 1., Binary, 0., "binary");
    const auto continuous = original.add_var(0., 10., Continuous, 0., "continuous");

    Model relaxation = original.copy();
    relaxation.set_var_type(integer, Continuous);
    relaxation.set_var_type(binary, Continuous);
    relaxation.add_ctr(integer == 2.25);
    relaxation.add_ctr(binary == 0.8);
    relaxation.add_ctr(continuous == 3.4);
    relaxation.use(OPTIMIZER());
    relaxation.optimize();
    REQUIRE(relaxation.get_status() == Optimal);

    DefaultNodeInfo info;
    info.save(original, relaxation);
    CHECK(info.sum_of_infeasibilities() == Catch::Approx(0.45));
}

template<class NodeSelectionRuleT, class BranchingRuleT>
void solve_with_branch_and_bound(const NodeSelectionRuleT& t_node_selection, const BranchingRuleT& t_branching_rule) {
    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(3), 0., 1., Binary, 0., "x");
    model.add_ctr(x[0] + 2 * x[1] + 2.5 * x[2] <= 4.);
    model.add_ctr(x[0] + x[1] >= 1.);
    model.set_obj_expr(-x[0] - x[1] - 2 * x[2]);
    model.use(BranchAndBound<DefaultNodeInfo>()
            .with_node_optimizer(OPTIMIZER::ContinuousRelaxation())
            .with_node_selection_rule(t_node_selection)
            .with_branching_rule(t_branching_rule));
    model.optimize();

    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == Catch::Approx(-3.));
    CHECK(model.get_var_primal(x[0]) == Catch::Approx(1.));
    CHECK(model.get_var_primal(x[1]) == Catch::Approx(0.));
    CHECK(model.get_var_primal(x[2]) == Catch::Approx(1.));
}

TEST_CASE("BranchAndBound runs with BestEstimate and default node information", "[basic][solver][branch-and-bound][best-estimate]") {
    solve_with_branch_and_bound(BestEstimate(), MostInfeasible());
}

TEST_CASE("BranchAndBound runs with StrongBranching and default node information", "[basic][solver][branch-and-bound][strong-branching]") {
    solve_with_branch_and_bound(BestBound(), StrongBranching());
}
