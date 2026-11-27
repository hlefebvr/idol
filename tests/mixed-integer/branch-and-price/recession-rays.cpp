#include <catch2/catch_all.hpp>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/mixed-integer/optimizers/callbacks/ReducedCostFixing.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/DualPriceSmoothingStabilization.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/general/optimizers/LambdaOptimizer/LambdaOptimizer.h"
#include "idol/general/optimizers/LambdaOptimizer/LambdaContext.h"

using namespace idol;
using namespace Catch::literals;

namespace {

class UnboundedSmoothing : public DantzigWolfe::DualPriceSmoothingStabilization {
    Ctr m_linking;
public:
    explicit UnboundedSmoothing(Ctr t_linking) : m_linking(std::move(t_linking)) {}

    class Strategy : public DualPriceSmoothingStabilization::Strategy {
        Ctr m_linking;
    public:
        explicit Strategy(Ctr t_linking) : m_linking(std::move(t_linking)) {}
        void initialize() override {}
        void update_stability_center(const DualPoint&) override {}
        DualPoint compute_smoothed_dual_solution(const DualPoint& t_master_dual) override {
            DualPoint result = t_master_dual;
            result.set(m_linking, 2.);
            return result;
        }
    };

    Strategy* operator()() const override { return new Strategy(m_linking); }
    UnboundedSmoothing* clone() const override { return new UnboundedSmoothing(*this); }
};

class PointHidingSmoothing : public DantzigWolfe::DualPriceSmoothingStabilization {
    Ctr m_linking;
    std::shared_ptr<unsigned int> m_n_calls;
public:
    PointHidingSmoothing(Ctr t_linking, std::shared_ptr<unsigned int> t_n_calls)
        : m_linking(std::move(t_linking)), m_n_calls(std::move(t_n_calls)) {}

    class Strategy : public DualPriceSmoothingStabilization::Strategy {
        Ctr m_linking;
        std::shared_ptr<unsigned int> m_n_calls;
    public:
        Strategy(Ctr t_linking, std::shared_ptr<unsigned int> t_n_calls)
            : m_linking(std::move(t_linking)), m_n_calls(std::move(t_n_calls)) {}
        void initialize() override {}
        void update_stability_center(const DualPoint&) override {}
        DualPoint compute_smoothed_dual_solution(const DualPoint& t_master_dual) override {
            ++*m_n_calls;
            DualPoint result = t_master_dual;
            result.set(m_linking, 1000.);
            return result;
        }
    };

    Strategy* operator()() const override { return new Strategy(m_linking, m_n_calls); }
    PointHidingSmoothing* clone() const override { return new PointHidingSmoothing(*this); }
};

PrimalPoint ray_with(const Var& t_var, double t_value) {
    PrimalPoint result;
    result.set_status(Unbounded);
    result.set_reason(Proved);
    result.set_objective_value(-Inf);
    result.set(t_var, t_value);
    return result;
}

Model make_one_block_model(Env& t_env,
                           Annotation<unsigned int>& t_decomposition,
                           const Var& t_x) {
    Model model(t_env);
    t_x.set(t_decomposition, 0);
    model.add(t_x);
    return model;
}

}

TEST_CASE("Generation patterns distinguish points from recession rays", "[ray][formulation]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Var x(env, 0., Inf, Continuous, 3., "x");
    Model model = make_one_block_model(env, decomposition, x);
    Ctr linking(env, 2. * x >= 0., "linking");
    model.add(linking);
    model.use(Gurobi());

    DantzigWolfe::Formulation formulation(model, decomposition);
    formulation.add_aggregation_constraint(0, 1., 1.);

    GenerationPattern<Var> affine_pattern(2. * x + 7., {});
    const auto direction = ray_with(x, 5.);
    CHECK(affine_pattern.generate_constant(direction) == 17._a);
    CHECK(affine_pattern.generate_directional_constant(direction) == 10._a);

    PrimalPoint point;
    point.set_status(Optimal);
    point.set(x, 4.);
    formulation.generate_column(0, point);
    formulation.generate_ray(0, direction);

    std::optional<Var> point_alpha;
    std::optional<Var> ray_alpha;
    unsigned int index = 0;
    for (const auto& [alpha, generator] : formulation.present_generators(0)) {
        (index++ == 0 ? point_alpha : ray_alpha) = alpha;
    }

    REQUIRE(point_alpha.has_value());
    REQUIRE(ray_alpha.has_value());
    CHECK(formulation.master().get_var_obj(*point_alpha) == 12._a);
    CHECK(formulation.master().get_var_obj(*ray_alpha) == 15._a);
    CHECK(formulation.master().get_mat_coeff(linking, *point_alpha) == 8._a);
    CHECK(formulation.master().get_mat_coeff(linking, *ray_alpha) == 10._a);
    CHECK(formulation.master().get_var_lb(*ray_alpha) == 0._a);
    CHECK(is_pos_inf(formulation.master().get_var_ub(*ray_alpha)));
    CHECK(formulation.master().get_var_type(*ray_alpha) == Continuous);

    DualPoint dual;
    dual.set(linking, 2.);
    for (const auto& ctr : formulation.master().ctrs()) {
        if (ctr.id() == linking.id()) { continue; }
        CHECK(formulation.master().get_mat_coeff(ctr, *point_alpha) == 1._a);
        CHECK(formulation.master().get_mat_coeff(ctr, *ray_alpha) == 0._a);
        dual.set(ctr, 100.);
    }
    CHECK(formulation.compute_ray_reduced_cost(0, dual, direction, false) == -5._a);
    CHECK(formulation.compute_ray_reduced_cost(0, dual, direction, true) == -20._a);
    CHECK_THROWS(formulation.generate_column(0, direction));
}

TEST_CASE("Point reduced costs match the explicit master column", "[reduced-cost][formulation]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Model model(env);
    const auto x = model.add_var(0., 2., Continuous, -3., "x");
    x.set(decomposition, 0);
    model.set_obj_const(-7.);
    const auto linking = model.add_ctr(2. * x >= 1., "linking");

    DantzigWolfe::Formulation formulation(model, decomposition);
    formulation.add_aggregation_constraint(0, 1., 1.);

    std::optional<Ctr> aggregation;
    for (const auto& [ctr, coefficient] : formulation.generation_pattern(x).linear()) {
        if (ctr.id() != linking.id() && coefficient.constant() == 1.) {
            aggregation = ctr;
        }
    }

    DualPoint dual;
    dual.set(linking, 4.);
    REQUIRE(aggregation.has_value());
    dual.set(*aggregation, 5.);
    PrimalPoint point;
    point.set_status(Optimal);
    point.set(x, 2.);

    const auto pricing_objective = formulation.compute_sub_problem_objective(0, dual);
    CHECK(evaluate(pricing_objective, point) == -27._a);
    CHECK(formulation.compute_reduced_cost(0, dual, point) == -27._a);
    CHECK(formulation.compute_sub_problem_variable_cost(0, dual, x) == -11._a);

    formulation.generate_column(0, point);
    const auto [alpha, generator] = *formulation.present_generators(0).begin();
    CHECK(formulation.master().get_obj_expr().affine().constant() == -7._a);
    CHECK(formulation.master().get_var_obj(alpha) == -6._a);
    CHECK(formulation.master().get_mat_coeff(linking, alpha) == 4._a);
    CHECK(formulation.master().get_mat_coeff(*aggregation, alpha) == 1._a);
    CHECK(formulation.master().get_var_obj(alpha)
          - 4. * formulation.master().get_mat_coeff(linking, alpha)
          - 5. * formulation.master().get_mat_coeff(*aggregation, alpha) == -27._a);

    Model direct = model.copy();
    direct.use(Gurobi::ContinuousRelaxation());
    direct.optimize();
    formulation.master().use(Gurobi::ContinuousRelaxation());
    formulation.master().optimize();
    CHECK(direct.get_best_obj() == -13._a);
    CHECK(formulation.master().get_best_obj() == direct.get_best_obj());
}

TEST_CASE("A valid bound from non-optimal pricing proves the CG bound", "[reduced-cost][pricing-bound]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Model model(env);
    const auto x = model.add_var(0., 1., Binary, -2., "x");
    x.set(decomposition, 0);

    const LambdaOptimizer enumerated_pricing([](LambdaContext& context) {
        const auto& pricing = context.get_model();
        const auto var = *pricing.vars().begin();
        PrimalPoint zero;
        zero.set(var, 0.);
        PrimalPoint one;
        one.set(var, 1.);
        const double value_zero = pricing.get_var_lb(var) <= 0. && pricing.get_var_ub(var) >= 0.
                                  ? evaluate(pricing.get_obj_expr(), zero) : Inf;
        const double value_one = pricing.get_var_lb(var) <= 1. && pricing.get_var_ub(var) >= 1.
                                 ? evaluate(pricing.get_obj_expr(), one) : Inf;
        const double value = std::min(value_zero, value_one);
        context.set_var_primal(var, value_one < value_zero ? 1. : 0.);
        context.set_best_obj(value);
        context.set_best_bound(value);
        context.set_status(Feasible);
    });

    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(
                      DantzigWolfe::SubProblem().add_optimizer(enumerated_pricing))
                  .with_infeasibility_strategy(DantzigWolfe::ArtificialCosts()));
    model.optimize();

    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == -2._a);
    CHECK(model.get_best_bound() == -2._a);
    CHECK(model.get_var_primal(x) == 1._a);
    CHECK(model.get_var_reduced_cost(x) == -2._a);
}

TEST_CASE("Zero pricing correction with infinite multiplicity remains finite", "[reduced-cost][pricing-bound]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Model model(env);
    const auto x = model.add_var(0., 0., Continuous, 0., "x");
    x.set(decomposition, 0);
    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(
                      DantzigWolfe::SubProblem()
                          .with_multiplicities(1., Inf)
                          .add_optimizer(Gurobi::ContinuousRelaxation()))
                  .with_infeasibility_strategy(DantzigWolfe::ArtificialCosts()));
    model.optimize();

    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == 0._a);
    CHECK(model.get_best_bound() == 0._a);
}

TEST_CASE("Decomposed variable fixing uses a conditional pricing bound", "[reduced-cost][fixing]") {
    // The raw pricing coefficients are 100 for x and 10 for y.  The local
    // equation x + y = 1 makes the actual degradation for changing x from
    // zero to one equal to 100 - 10 = 90.
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Model model(env);
    const auto x = model.add_var(0., 1., Binary, 100., "x");
    const auto y = model.add_var(0., 1., Binary, 10., "y");
    x.set(decomposition, 0);
    y.set(decomposition, 0);
    const auto local = model.add_ctr(x + y == 1., "local");
    local.set(decomposition, 0);

    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(
                      DantzigWolfe::SubProblem().add_optimizer(Gurobi()))
                  .with_infeasibility_strategy(DantzigWolfe::ArtificialCosts()));
    model.optimize();

    REQUIRE(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == 10._a);
    CHECK(model.get_var_primal(x) == 0._a);
    CHECK(model.get_var_primal(y) == 1._a);
    CHECK(model.get_var_reduced_cost(x) == 90._a);
    CHECK(model.get_var_reduced_cost(y) == -90._a);
}

TEST_CASE("Reduced-cost fixing queries decomposed original variables", "[reduced-cost][fixing][branch-and-price]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Model model(env);
    const auto x = model.add_var(0., 1., Binary, 100., "x");
    const auto y = model.add_var(0., 1., Binary, 10., "y");
    const auto z = model.add_var(0., 1., Binary, 1., "z");
    x.set(decomposition, 0);
    y.set(decomposition, 0);
    const auto local = model.add_ctr(x + y == 1., "local");
    local.set(decomposition, 0);
    model.add_ctr(x + 2. * z >= 1.5, "linking");

    auto n_conditional_pricing_calls = std::make_shared<unsigned int>(0);
    const LambdaOptimizer pricing([=](LambdaContext& context) {
        const auto& sub_problem = context.get_model();
        if (sub_problem.get_var_lb(x) > .5 || sub_problem.get_var_ub(y) < .5) {
            ++*n_conditional_pricing_calls;
        }
        double best = Inf;
        double best_x = 0.;
        double best_y = 0.;
        for (double x_value : {0., 1.}) {
            for (double y_value : {0., 1.}) {
                if (x_value < sub_problem.get_var_lb(x) || x_value > sub_problem.get_var_ub(x) ||
                    y_value < sub_problem.get_var_lb(y) || y_value > sub_problem.get_var_ub(y) ||
                    !equals(x_value + y_value, 1., 1e-9)) {
                    continue;
                }
                PrimalPoint point;
                point.set(x, x_value);
                point.set(y, y_value);
                const double value = evaluate(sub_problem.get_obj_expr(), point);
                if (value < best) {
                    best = value;
                    best_x = x_value;
                    best_y = y_value;
                }
            }
        }
        if (is_pos_inf(best)) {
            context.set_status(Infeasible);
            return;
        }
        context.set_var_primal(x, best_x);
        context.set_var_primal(y, best_y);
        context.set_best_obj(best);
        context.set_best_bound(best);
        context.set_status(Optimal);
    });

    const auto column_generation = DantzigWolfeDecomposition(decomposition)
        .with_master_optimizer(Gurobi::ContinuousRelaxation())
        .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(pricing))
        .with_infeasibility_strategy(DantzigWolfe::ArtificialCosts());
    const auto branch_and_bound = BranchAndBound<DefaultNodeInfo>()
        .with_branching_rule(MostInfeasible())
        .with_node_selection_rule(BestBound())
        .add_callback(Heuristics::IntegerMaster<>().with_optimizer(Gurobi()))
        .add_callback(ReducedCostFixing<>());
    model.use(branch_and_bound + column_generation);
    model.optimize();

    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == 11._a);
    CHECK(model.get_var_primal(x) == 0._a);
    CHECK(model.get_var_primal(y) == 1._a);
    CHECK(model.get_var_primal(z) == 1._a);
    CHECK(*n_conditional_pricing_calls > 0);
}

TEST_CASE("Recession directions use homogeneous rows and bounds", "[ray][recession]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Model model(env);
    Var lower(env, 0., Inf, Continuous, 0., "lower");
    Var upper(env, -Inf, 5., Continuous, 0., "upper");
    Var boxed(env, 0., 5., Continuous, 0., "boxed");
    Var free(env, -Inf, Inf, Continuous, 0., "free");
    for (const auto& var : {lower, upper, boxed, free}) {
        var.set(decomposition, 0);
        model.add(var);
    }
    Ctr le(env, lower + upper <= 4.);
    Ctr ge(env, lower - upper >= -3.);
    Ctr eq(env, lower + upper == 2.);
    for (const auto& ctr : {le, ge, eq}) {
        ctr.set(decomposition, 0);
        model.add(ctr);
    }
    model.use(Gurobi());
    DantzigWolfe::Formulation formulation(model, decomposition);

    PrimalPoint compatible = ray_with(lower, 1.);
    compatible.set(upper, -1.);
    compatible.set(boxed, 0.);
    compatible.set(free, 4.);
    CHECK(formulation.is_recession_direction(compatible, 0));

    auto bad_lower = compatible;
    bad_lower.set(lower, -1.);
    CHECK_FALSE(formulation.is_recession_direction(bad_lower, 0));
    auto bad_upper = compatible;
    bad_upper.set(upper, 1.);
    CHECK_FALSE(formulation.is_recession_direction(bad_upper, 0));
    auto bad_boxed = compatible;
    bad_boxed.set(boxed, 1.);
    CHECK_FALSE(formulation.is_recession_direction(bad_boxed, 0));
    auto bad_equality = compatible;
    bad_equality.set(upper, -2.);
    CHECK_FALSE(formulation.is_recession_direction(bad_equality, 0));
}

TEST_CASE("Column generation uses an unbounded pricing ray to solve a bounded model", "[ray][bounded]") {
    // min x, x >= 1, x in X = {x >= 0}. The pricing recession ray is r = 1.
    // Its master column has objective 1, linking coefficient 1, and multiplicity 0.
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Var x(env, 0., Inf, Continuous, 1., "x");
    Model model = make_one_block_model(env, decomposition, x);
    Ctr linking(env, x >= 1., "linking");
    model.add(linking);
    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(Gurobi()))
                  .with_infeasibility_strategy(DantzigWolfe::FarkasPricing()));

    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == 1._a);
    CHECK(model.get_var_primal(x) == 1._a);

    const auto& dw = model.optimizer().as<Optimizers::DantzigWolfeDecomposition>();
    bool found_ray = false;
    for (const auto& [alpha, generator] : dw.formulation().present_generators(0)) {
        if (generator.status() != Unbounded) { continue; }
        found_ray = true;
        CHECK(dw.formulation().master().get_mat_coeff(linking, alpha) == 1._a);
        for (const auto& ctr : dw.formulation().master().ctrs()) {
            if (ctr.id() != linking.id()) {
                CHECK(dw.formulation().master().get_mat_coeff(ctr, alpha) == 0._a);
            }
        }
    }
    CHECK(found_ray);
}

TEST_CASE("Column generation reconstructs an original-space unbounded ray", "[ray][unbounded]") {
    // min -x, x in X = {x >= 0}. Direction r = 1 is feasible and has cost -1.
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Var x(env, 0., Inf, Continuous, -1., "x");
    Model model = make_one_block_model(env, decomposition, x);
    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(Gurobi()))
                  .with_infeasibility_strategy(DantzigWolfe::FarkasPricing()));

    model.optimize();
    REQUIRE(model.get_status() == Unbounded);
    const double ray = model.get_var_ray(x);
    CHECK(ray >= 0.);
    CHECK(-ray < 0.);
}

TEST_CASE("Optional zero-point-mass blocks reject recession rays", "[ray][multiplicity]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Var x(env, 0., Inf, Continuous, -1., "x");
    Model model = make_one_block_model(env, decomposition, x);
    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(
                      DantzigWolfe::SubProblem().with_multiplicities(0., 1.).add_optimizer(Gurobi()))
                  .with_infeasibility_strategy(DantzigWolfe::FarkasPricing()));

    CHECK_THROWS_WITH(model.optimize(),
                      "Ray generation for an optional block with zero point multiplicity is not currently supported.");
}

TEST_CASE("Artificial costs propagate an unbounded master ray", "[ray][artificial-costs]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Var x(env, 0., Inf, Continuous, -1., "x");
    Model model = make_one_block_model(env, decomposition, x);
    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(Gurobi()))
                  .with_infeasibility_strategy(DantzigWolfe::ArtificialCosts()));

    model.optimize();
    REQUIRE(model.get_status() == Unbounded);
    CHECK(model.get_var_ray(x) > 0.);
}

TEST_CASE("Hard branching applies homogeneous bound rules to rays", "[ray][branching]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Var x(env, 0., Inf, Continuous, 0., "x");
    Model model = make_one_block_model(env, decomposition, x);
    model.use(Gurobi());
    DantzigWolfe::Formulation formulation(model, decomposition);
    formulation.generate_ray(0, ray_with(x, 1.));

    Var alpha = formulation.present_generators(0).begin()->first;
    formulation.update_var_lb(x, 2., true, true);
    CHECK(formulation.master().has(alpha));

    formulation.update_var_ub(x, 5., true, true);
    CHECK_FALSE(formulation.master().has(alpha));
    formulation.load_columns_from_pool();
    CHECK_FALSE(formulation.master().has(alpha));
}

TEST_CASE("A pooled ray is reloaded directionally on a second solve", "[ray][pool]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Var x(env, 0., Inf, Continuous, 1., "x");
    Model model = make_one_block_model(env, decomposition, x);
    Ctr linking(env, x >= 1., "linking");
    model.add(linking);
    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(Gurobi()))
                  .with_infeasibility_strategy(DantzigWolfe::FarkasPricing()));

    model.optimize();
    REQUIRE(model.get_status() == Optimal);
    auto& formulation = model.optimizer().as<Optimizers::DantzigWolfeDecomposition>().formulation();
    std::optional<Var> ray_alpha;
    formulation.remove_column_if(0, [&](const Var& alpha, const PrimalPoint& generator) {
        if (generator.status() == Unbounded) { ray_alpha = alpha; return true; }
        return false;
    });
    REQUIRE(ray_alpha.has_value());
    REQUIRE_FALSE(formulation.master().has(*ray_alpha));

    model.optimize();
    REQUIRE(model.get_status() == Optimal);
    REQUIRE(formulation.master().has(*ray_alpha));
    CHECK(formulation.master().get_var_obj(*ray_alpha) == 1._a);
    CHECK(formulation.master().get_mat_coeff(linking, *ray_alpha) == 1._a);
    for (const auto& ctr : formulation.master().ctrs()) {
        if (ctr.id() != linking.id()) {
            CHECK(formulation.master().get_mat_coeff(ctr, *ray_alpha) == 0._a);
        }
    }
}

TEST_CASE("A stabilized non-improving ray triggers raw-dual repricing", "[ray][stabilization]") {
    // At the finite optimum pi = 1. The test smoother changes pi to 2, making
    // min (1-pi)x unbounded along r=1, while the raw directional cost is zero.
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Var x(env, 0., Inf, Continuous, 1., "x");
    Model model = make_one_block_model(env, decomposition, x);
    Ctr linking(env, x >= 1., "linking");
    model.add(linking);
    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(Gurobi()))
                  .with_infeasibility_strategy(DantzigWolfe::FarkasPricing())
                  .with_dual_price_smoothing_stabilization(UnboundedSmoothing(linking)));

    model.optimize();
    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == 1._a);
}

TEST_CASE("A stabilized non-improving point triggers raw-dual repricing", "[reduced-cost][stabilization]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Model model(env);
    const auto a = model.add_var(0., 1., Binary, 100., "a");
    const auto b = model.add_var(0., 1., Binary, 0., "b");
    a.set(decomposition, 0);
    b.set(decomposition, 0);
    const auto local = model.add_ctr(a + b == 1., "local");
    local.set(decomposition, 0);
    const auto linking = model.add_ctr(2. * a + b >= 1., "linking");
    auto n_smoothing_calls = std::make_shared<unsigned int>(0);

    model.use(DantzigWolfeDecomposition(decomposition)
                  .with_master_optimizer(Gurobi::ContinuousRelaxation())
                  .with_default_sub_problem_spec(
                      DantzigWolfe::SubProblem().add_optimizer(Gurobi()))
                  .with_infeasibility_strategy(DantzigWolfe::ArtificialCosts())
                  .with_dual_price_smoothing_stabilization(
                      PointHidingSmoothing(linking, n_smoothing_calls)));

    PrimalPoint initial;
    initial.set_status(Optimal);
    initial.set(a, 1.);
    initial.set(b, 0.);
    model.optimizer().as<Optimizers::DantzigWolfeDecomposition>()
        .formulation().generate_column(0, std::move(initial));

    model.optimize();

    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == 0._a);
    CHECK(model.get_var_primal(a) == 0._a);
    CHECK(model.get_var_primal(b) == 1._a);
    CHECK(*n_smoothing_calls <= 3);
}

TEST_CASE("Integer master uses nonnegative unbounded integer ray multipliers", "[ray][integer-master]") {
    Env env;
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    Var x(env, 0., Inf, Integer, 0., "x");
    Model model = make_one_block_model(env, decomposition, x);
    model.use(Gurobi());
    DantzigWolfe::Formulation formulation(model, decomposition);
    const auto ray = ray_with(x, 1.);
    formulation.generate_ray(0, ray);
    const Var alpha = formulation.present_generators(0).begin()->first;
    formulation.master().set_var_type(alpha, Heuristics::IntegerMaster<>::generator_type(ray));

    CHECK(formulation.master().get_var_type(alpha) == Integer);
    CHECK(formulation.master().get_var_lb(alpha) == 0._a);
    CHECK(is_pos_inf(formulation.master().get_var_ub(alpha)));
}
