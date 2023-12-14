#include "idol/modeling.h"
#include "idol/problems/knapsack-problem/KP_Instance.h"
#include "idol/modeling/models/dualize.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"

using namespace idol;

int main(int t_argc, char** t_argv) {

    Env env;

    auto model = Gurobi::read_from_file(env, "/home/henri/Research/bilevel-ccg/code/cmake-build-debug/instance.lp");

    std::cout << model << std::endl;

    return 0;
}
