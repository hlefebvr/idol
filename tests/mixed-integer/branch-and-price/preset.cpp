#include <catch2/catch_all.hpp>

#include <limits>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/presets/BranchAndPrice.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"

using namespace idol;

namespace {

std::pair<Model, Annotation<unsigned int>> make_model(Env& t_env) {
    Model model(t_env);
    Annotation<unsigned int> decomposition(t_env, "decomposition", MasterId);
    const auto x = model.add_vars(Dim<2>(2, 2), 0., 1., Binary, 1., "x");

    const auto block_0 = model.add_ctr(x[0][0] + x[0][1] >= 1, "block_0");
    const auto block_1 = model.add_ctr(x[1][0] + x[1][1] >= 1, "block_1");
    block_0.set(decomposition, 0);
    block_1.set(decomposition, 1);
    model.add_ctr(x[0][0] + x[1][0] <= 1, "linking_0");
    model.add_ctr(x[0][1] + x[1][1] <= 1, "linking_1");

    return { std::move(model), decomposition };
}

}

TEST_CASE("The branch-and-price preset matches its explicit configuration", "[preset][GLPK]") {
    Env env;
    auto [preset_model, decomposition] = make_model(env);
    auto explicit_model = preset_model.copy();

    preset_model.use(Presets::BranchAndPrice(
            decomposition, GLPK::ContinuousRelaxation(), GLPK()));

    const auto column_generation = DantzigWolfeDecomposition(decomposition)
            .with_master_optimizer(GLPK::ContinuousRelaxation())
            .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(GLPK()))
            .with_dual_price_smoothing_stabilization(DantzigWolfe::Wentges(.4));
    const auto branch_and_bound = BranchAndBound<NodeWithCGInfo>()
            .with_branching_rule(MostInfeasible())
            .with_node_selection_rule(BestBound());
    explicit_model.use(branch_and_bound + column_generation);

    preset_model.optimize();
    explicit_model.optimize();

    REQUIRE(preset_model.get_status() == Optimal);
    REQUIRE(explicit_model.get_status() == Optimal);
    CHECK(preset_model.get_best_obj() == Catch::Approx(2.));
    CHECK(preset_model.get_best_obj() == Catch::Approx(explicit_model.get_best_obj()));
}

TEST_CASE("Branch-and-price preset factories clone independently and remain customizable", "[preset][GLPK]") {
    Env env;
    auto [first_model, decomposition] = make_model(env);
    auto second_model = first_model.copy();

    const auto original = Presets::BranchAndPrice(
            decomposition, GLPK::ContinuousRelaxation(), GLPK());
    auto customized = original;
    customized.with_time_limit(17.).with_logs(false);

    first_model.use(original);
    second_model.use(customized);

    CHECK(first_model.optimizer().get_param_time_limit() == std::numeric_limits<double>::max());
    CHECK(second_model.optimizer().get_param_time_limit() == 17.);
    CHECK_FALSE(second_model.optimizer().get_param_logs());

    first_model.optimize();
    second_model.optimize();
    CHECK(first_model.get_best_obj() == Catch::Approx(second_model.get_best_obj()));
}

TEST_CASE("The branch-and-price preset supports another pricing backend", "[preset][HiGHS]") {
    Env env;
    auto [model, decomposition] = make_model(env);
    model.use(Presets::BranchAndPrice(
            decomposition, GLPK::ContinuousRelaxation(), HiGHS()));
    model.optimize();

    REQUIRE(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == Catch::Approx(2.));
}
