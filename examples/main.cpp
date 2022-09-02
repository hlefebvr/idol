#include <iostream>
#include "../lib/include/modeling/Model.h"
#include "../lib/include/modeling/Env.h"
#include "../lib/include/modeling/LinExpr.h"
#include "../lib/include/modeling/Expr.h"

#include <unordered_map>

int main() {

    Env env;

    Model model(env);

    auto x = model.create_variable(0., Inf, Continuous, "x");
    auto y = model.create_variable(0., Inf, Continuous, "y");
    auto xi = model.create_parameter(0., Inf, Continuous, "xi");

    // LinExpr

    auto* lin_expr = new LinExpr<Decision>();

    // *lin_expr = x * 3; forbidden
    *lin_expr = 3 * x; std::cout << *lin_expr << std::endl;

    *lin_expr = x + 2; std::cout << *lin_expr << std::endl;
    *lin_expr = 2 + x; std::cout << *lin_expr << std::endl;

    *lin_expr = x + x; std::cout << *lin_expr << std::endl;

    *lin_expr = x + 2 * x; std::cout << *lin_expr << std::endl;
    *lin_expr = (2 * x) + x; std::cout << *lin_expr << std::endl;

    *lin_expr = 1 + 2 * x; std::cout << *lin_expr << std::endl;
    *lin_expr = 2 * x + 1; std::cout << *lin_expr << std::endl;

    *lin_expr = 2 * x + 2 * x; std::cout << *lin_expr << std::endl;

    *lin_expr = 2 * x + y + 10 + 4 * x + 2 * y; std::cout << *lin_expr << std::endl;

    // Expr

    auto* expr = new Expr<Decision>();

    *expr = xi * x; std::cout << *expr << std::endl;
    // *expr = x * xi; gives an Expr<Parameter>

    *expr = (2 * xi) * x; std::cout << *expr << std::endl;
    // *expr = x * (2 * xi); forbidden

    *expr = (1 + 2 * xi) * x + 1; std::cout << *expr << std::endl;
    *expr = 1 + (1 + 2 * xi) * x; std::cout << *expr << std::endl;

    *expr = (1 + 2 * xi) * x + x; std::cout << *expr << std::endl;
    *expr = x + (1 + 2 * xi) * x; std::cout << *expr << std::endl;

    *expr = (1 + 2 * xi) * x + xi; std::cout << *expr << std::endl;
    *expr = xi + (1 + 2 * xi) * x; std::cout << *expr << std::endl;

    *expr = (2 * xi) * x + (2 * xi) * x; std::cout << *expr << std::endl;

    *expr = x + 2; *lin_expr = x; std::cout << (*expr + *lin_expr) << std::endl;

    delete lin_expr;
    delete expr;

    return 0;
}
