#include <iostream>
#include "idol/containers/SparseVector.h"
#include "idol/modeling/variables/Var.h"
#include "idol/modeling/models/Model.h"
#include "idol/modeling/objects/Env.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;
    Model model(env);
    const auto x = model.add_vars(Dim<1>(10), 1., 0., Continuous, "x");

    SparseVector<Var, double> v1;
    SparseVector<Var, double> v2;

    for (unsigned int i = 0 ; i < 10 ; ++i) {
        v1.push_back(x[i], i);
        //v1.push_back(x[i], 10 - i);
    }
    for (unsigned int i = 0 ; i < 10 ; ++i) {
        v2.push_back(x[i], i);
        v2.push_back(x[i], 10 - i);
    }
    std::cout << v1.is_reduced() << std::endl;
    std::cout << v2.is_reduced() << std::endl;
    std::cout << v1.is_sorted_by_index() << std::endl;
    std::cout << v2.is_sorted_by_index() << std::endl;

    auto sum = v1;
    sum += v2;

    std::cout << sum << std::endl;

    return 0;
}
