#include <catch2/catch_test_macros.hpp>

#include "idol/modeling.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/bilevel/optimizers/BranchAndCut/BranchAndCut.h"
#include "idol/robust/modeling/Description.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"

using namespace idol;

TEST_CASE("Environment keeps infeasible-or-unbounded information separate from the best-bound stop", "[basic][environment]") {
    Env env;
    const double best_bound_stop = 42.;
    env.set_param_best_bound_stop(best_bound_stop);

    env.set_param_infeasible_or_unbounded_info(true);
    CHECK(env.get_param_infeasible_or_unbounded_info());
    CHECK(env.get_param_best_bound_stop() == best_bound_stop);

    env.set_param_infeasible_or_unbounded_info(false);
    CHECK_FALSE(env.get_param_infeasible_or_unbounded_info());
    CHECK(env.get_param_best_bound_stop() == best_bound_stop);
}

TEST_CASE("Composite optimizer clones retain common factory parameters", "[basic][optimizer-factory]") {
    Env env;
    Model uncertainty_set(env);
    Bilevel::Description bilevel_description(env);
    Robust::Description robust_description(uncertainty_set);

    SECTION("bilevel branch-and-cut") {
        Bilevel::BranchAndCut factory(bilevel_description);
        factory.with_logs(true);
        std::unique_ptr<OptimizerFactory> clone(factory.clone());
        CHECK_THROWS(clone->as<Bilevel::BranchAndCut>().with_logs(false));
    }

    SECTION("column-and-constraint generation") {
        Robust::ColumnAndConstraintGeneration factory(robust_description, bilevel_description);
        factory.with_logs(true);
        std::unique_ptr<OptimizerFactory> clone(factory.clone());
        CHECK_THROWS(clone->as<Robust::ColumnAndConstraintGeneration>().with_logs(false));
    }
}

TEST_CASE("Basic expression algebra preserves coefficients", "[basic][expressions]") {
    Env env;
    const Var x(env, -Inf, Inf, Continuous, 0., "x");
    const Var y(env, -Inf, Inf, Continuous, 0., "y");

    const LinExpr<Var> linear = 2 * x - 3 * y + x;
    CHECK(linear.get(x) == 3.);
    CHECK(linear.get(y) == -3.);

    const AffExpr<Var> affine = 5 + 2 * linear - x;
    CHECK(affine.constant() == 5.);
    CHECK(affine.linear().get(x) == 5.);
    CHECK(affine.linear().get(y) == -6.);

    const auto summed = idol_Sum(i, Range(3), (i + 1.) * x);
    CHECK(summed.get(x) == 6.);

    const QuadExpr<Var> quadratic = 2 * x * x + 3 * x * y - y * x + 4 * y + 7;
    CHECK(quadratic.get(CommutativePair<Var>(x, x)) == 2.);
    CHECK(quadratic.get(CommutativePair<Var>(x, y)) == 2.);
    CHECK(quadratic.affine().linear().get(y) == 4.);
    CHECK(quadratic.affine().constant() == 7.);
}

TEST_CASE("Variables have independent model-specific versions", "[basic][variables][model-state]") {
    Env env;
    const Var shared(env, -2., 8., Integer, 3., "shared");
    Model first(env);
    Model second(env);
    first.add(shared);
    second.add(shared);

    first.set_var_lb(shared, 1.);
    first.set_var_ub(shared, 5.);
    first.set_var_type(shared, Binary);
    first.set_var_obj(shared, 9.);

    CHECK(shared.name() == "shared");
    CHECK(first.get_var_lb(shared) == 1.);
    CHECK(first.get_var_ub(shared) == 5.);
    CHECK(first.get_var_type(shared) == Binary);
    CHECK(first.get_var_obj(shared) == 9.);
    CHECK(second.get_var_lb(shared) == -2.);
    CHECK(second.get_var_ub(shared) == 8.);
    CHECK(second.get_var_type(shared) == Integer);
    CHECK(second.get_var_obj(shared) == 3.);

    const auto vector = first.add_vars(Dim<1>(3), 0., 1., Continuous, 2., "v");
    CHECK(vector.size() == 3);
    CHECK(first.get_var_type(vector[1]) == Continuous);
    CHECK(first.get_var_obj(vector[1]) == 2.);
    CHECK(vector[1].name() == "v_1");
}

TEST_CASE("Linear constraints expose and update model state", "[basic][constraints]") {
    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(2), 0., Inf, Continuous, 0., "x");
    const auto le = model.add_ctr(2 * x[0] + x[1] <= 7, "le");
    const auto ge = model.add_ctr(x[0] - x[1] >= -2, "ge");
    const auto eq = model.add_ctr(x[0] + x[1] == 3, "eq");

    CHECK(model.get_ctr_type(le) == LessOrEqual);
    CHECK(model.get_ctr_type(ge) == GreaterOrEqual);
    CHECK(model.get_ctr_type(eq) == Equal);
    CHECK(model.get_ctr_rhs(le) == 7.);
    CHECK(model.get_mat_coeff(le, x[0]) == 2.);

    model.set_ctr_rhs(le, 8.);
    model.set_ctr_type(le, Equal);
    model.set_mat_coeff(le, x[1], 4.);
    CHECK(model.get_ctr_rhs(le) == 8.);
    CHECK(model.get_ctr_type(le) == Equal);
    CHECK(model.get_ctr_row(le).get(x[1]) == 4.);
    CHECK(model.get_var_column(x[1]).get(le) == 4.);

    model.remove(ge);
    CHECK_FALSE(model.has(ge));
}

TEST_CASE("Objective state supports affine and quadratic expressions", "[basic][objective]") {
    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(2), -Inf, Inf, Continuous, 0., "x");

    model.set_obj_expr(2 * x[0] - x[1] + 4);
    CHECK(model.get_obj_expr().affine().constant() == 4.);
    CHECK(model.get_var_obj(x[0]) == 2.);

    model.set_obj_expr(x[0] * x[0] + 3 * x[0] * x[1] + x[1] + 6);
    CHECK(model.get_obj_expr().get(CommutativePair<Var>(x[0], x[0])) == 1.);
    CHECK(model.get_obj_expr().get(CommutativePair<Var>(x[0], x[1])) == 3.);
    CHECK(model.get_obj_expr().affine().constant() == 6.);
    model.set_var_obj(x[1], 5.);
    CHECK(model.get_obj_expr().affine().linear().get(x[1]) == 5.);
}

TEST_CASE("Copying a model preserves and isolates its formulation", "[basic][model-copy]") {
    Env env;
    Model original(env);
    const auto x = original.add_var(0., 4., Integer, 2., "x");
    const auto c = original.add_ctr(3 * x <= 9, "capacity");
    Model copy = original.copy();
    CHECK(copy.has(x));
    CHECK(copy.has(c));
    CHECK(copy.get_var_ub(x) == 4.);
    CHECK(copy.get_ctr_rhs(c) == 9.);

    copy.set_var_ub(x, 2.);
    copy.set_ctr_rhs(c, 6.);
    copy.set_var_obj(x, 7.);
    CHECK(original.get_var_ub(x) == 4.);
    CHECK(original.get_ctr_rhs(c) == 9.);
    CHECK(original.get_var_obj(x) == 2.);
}
