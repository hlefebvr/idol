#include <iostream>
#include "modeling.h"

int main() {

    Env env;

    Model model(env);

    auto x = model.add_variable(0., Inf, Continuous);
    auto y = model.add_variable(0., Inf, Continuous);
    auto xi = model.add_parameter(0., Inf, Continuous, "xi");

    auto ctr = model.add_constraint(x + y <= 2.);

    std::cout << ctr << std::endl;

    return 0;
}
