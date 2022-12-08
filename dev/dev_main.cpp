#include <iostream>
#include "modeling.h"
#include "algorithms.h"


int main(int t_argc, const char** t_argv) {

    Model model;
    auto x = model.add_vars(Dim<1>(2), 0., 1., Binary, 0.);
    auto y = model.add_vars(Dim<1>(2), 0., 1., Binary, 0.);

    auto subproblem = model.add_user_attr(0, "subproblem");

    auto c1 = model.add_ctr(x[0] + x[1] >= 1);
    auto c2 = model.add_ctr(x[0] + y[1] <= 2);
    auto c3 = model.add_ctr(y[0] + y[1] >= 1);

    model.set(subproblem, c1, 1);

    std::cout << model.get(subproblem, c1) << std::endl;

    return 0;
}
