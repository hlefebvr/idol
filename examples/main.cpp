#include <iostream>
#include "modeling.h"
#include "modeling/environment/Env.h"
#include "modeling/models/Model.h"
#include "modeling/coefficients/Coefficient.h"
#include "modeling/coefficients/MatrixCoefficient.h"
#include "modeling/constraints/TempCtr.h"

int main() {

    Env env;

    Model model(env);

    auto xi_1 = model.add_parameter(0., 1., Continuous, "xi_1");
    auto xi_2 = model.add_parameter(0., 1., Continuous, "xi_2");
    auto x = model.add_variable(0., 1., Binary, 0, "x");
    auto y = model.add_variable(0., 1., Binary, 0, "y");



    return 0;
}

