#include <iostream>
#include "modeling.h"

int main() {

    Env env;

    Model model(env);

    auto xi_1 = model.add_parameter(0., 1., Continuous, "xi_1");
    auto xi_2 = model.add_parameter(0., 1., Continuous, "xi_2");
    auto x = model.add_variable(0., 1., Binary, 0, "x");
    auto y = model.add_variable(0., 1., Binary, 0, "y");

    auto c1 = model.add_constraint(x + y >= 1);
    auto ctr = model.add_constraint(x <= 1);

    std::cout << ctr.get(x).constant() << std::endl;
    std::cout << ctr.get(y).constant() << std::endl;
    std::cout << ctr.row().size() << std::endl;
    std::cout << x.column().size() << std::endl;

    model.update_coefficient(ctr, x, 3.);

    std::cout << std::endl;

    std::cout << x.get(ctr).constant() << std::endl;
    std::cout << x.column().get(ctr).constant() << std::endl;
    std::cout << ctr.get(x).constant() << std::endl;
    std::cout << ctr.row().get(x).constant() << std::endl;
    std::cout << ctr.get(y).constant() << std::endl;
    std::cout << ctr.row().size() << std::endl;
    std::cout << x.column().size() << std::endl;


    return 0;
}

