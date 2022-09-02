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

    auto x = model.create_variable(0., Inf, Continuous, "x");
    auto y = model.create_variable(0., Inf, Continuous, "y");
    auto xi = model.create_parameter(0., Inf, Continuous, "xi");

    return 0;
}
