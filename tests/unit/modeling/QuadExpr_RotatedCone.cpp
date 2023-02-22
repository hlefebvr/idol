//
// Created by henri on 22/02/23.
//

#ifdef IDOL_USE_EIGEN

#include "../../test_utils.h"
#include "linear-algebra/to_rotated_quadratic_cone.h"

double eval(const LinExpr<Var>& t_expr, const Solution::Primal& t_primal) {
    double result = 0;
    for (const auto& [var, constant] : t_expr) {
        result += constant.numerical() * t_primal.get(var);
    }
    return result;
}

double eval(const QuadExpr<Var, Var>& t_expr, const Solution::Primal& t_primal) {
    double result = 0;
    for (const auto& [var1, var2, constant] : t_expr) {
        result += constant.numerical() * t_primal.get(var1) * t_primal.get(var2);
    }
    return result;
}

double eval(const Expr<Var, Var>& t_expr, const Solution::Primal& t_primal) {
    return t_expr.constant().numerical() + eval(t_expr.linear(), t_primal) + eval(t_expr.quadratic(), t_primal);
}

TEST_CASE("QuadExpr: rotated cone expression", "[unit][modeling][QuadExpr]") {

    Env env;

    auto x = Var::array<1>(env, Dim<1>(4), 0., Inf, Continuous, "x");

    std::vector<QuadExpr<Var, Var>> quadratic_expressions = {
            x[0] * x[0] + x[1] * x[1],
            3 * x[0] * x[0] + 4 * x[1] * x[1],
            3 * x[0] * x[0] + x[1] * x[1] - 2 * x[0] * x[1],
            x[0] * x[0] + x[1] * x[1] - 2 * x[2] * x[3],
            3 * x[0] * x[0] + 4 * x[1] * x[1] + 2 * x[0] * x[1],
            x[0] * x[0] + x[1] * x[1] - x[2] * x[2]
    };

    auto expr = GENERATE_COPY(
                quadratic_expressions[0],
                quadratic_expressions[1],
                quadratic_expressions[2],
                quadratic_expressions[3],
                quadratic_expressions[4],
                quadratic_expressions[5]
            );

    std::stringstream ss;
    ss << expr;

    WHEN("Expressing " + ss.str() + " <= 0 in a rotated quadratic cone") {

        auto result = to_rotated_quadratic_cone(expr);

        THEN("Evaluations at 30 random points should match") {

            std::random_device rd;
            std::mt19937 engine(rd());
            std::uniform_real_distribution<double> dist(0, 10);

            for (unsigned int i = 0; i < 30; ++i) {

                Solution::Primal point;
                point.set_objective_value(0.);
                point.set(x[0], dist(engine));
                point.set(x[1], dist(engine));
                point.set(x[2], dist(engine));
                point.set(x[3], dist(engine));

                const double eval_expr = eval(expr, point);

                auto it = result.begin();

                double eval_result = -2. * eval(*it, point) * eval(*++it, point);
                ++it;
                for (auto end = result.end(); it != end; ++it) {
                    eval_result += eval(*it, point) * eval(*it, point);
                }

                CHECK(std::abs(eval_expr - eval_result) < 1e-5);

            }

        }

    }

}

#endif