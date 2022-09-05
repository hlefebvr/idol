#include <iostream>
#include "modeling.h"

int main() {

    Env env;

    Model model(env);

    Variable<Decision> x = model.add_variable(0., Inf, Continuous, "x");
    /*auto y = model.add_variable(0., Inf, Continuous, "y");
    auto xi = model.add_parameter(0., Inf, Continuous, "xi");

    auto ctr = model.add_constraint(x + y <= 2.);

    std::cout << ctr << std::endl;

    model.update(x, skip<double>, 10., skip<VariableType>);*/

    return 0;
}
