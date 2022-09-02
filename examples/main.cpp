#include <iostream>
#include "modeling/Model.h"
#include "modeling/Env.h"
#include "modeling/LinExpr.h"
#include "modeling/Expr.h"
#include "modeling/TempConstraint.h"

#include <unordered_map>

int main() {

    Env env;

    Model model(env);

    auto x = model.add_variable(0., Inf, Continuous, "x");
    auto y = model.add_variable(0., Inf, Continuous, "y");
    auto xi = model.add_parameter(0., Inf, Continuous, "xi");

    model.add_constraint(x + y <= 2.);

    Expr expr;

    expr += 1 * xi;
    expr += 2 * x;
    expr += 3 * y;
    expr += 2;

    std::cout << expr << std::endl;

    return 0;
}
