#include "idol/modeling.h"
#include "idol/problems/knapsack-problem/KP_Instance.h"
#include "idol/modeling/models/dualize.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"

using namespace idol;

int main(int t_argc, char** t_argv) {

    const auto instance = Problems::KP::read_instance("/home/henri/Research/idol/examples/knapsack.data.txt");

    const auto n_items = instance.n_items();

    Env env;

    // Create primal
    Model primal(env);

    auto x = primal.add_vars(Dim<1>(n_items), 0, 1, Continuous, "x");

    auto c = primal.add_ctr(idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    primal.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    primal.use(Gurobi());

    primal.optimize();

    auto dual = dualize(primal);

    std::cout << primal << std::endl;
    std::cout << dual << std::endl;

    dual.use(Gurobi().with_infeasible_or_unbounded_info(true));

    dual.optimize();

    std::cout << primal.get_status() << std::endl;
    std::cout << primal.get_best_obj() << std::endl;
    std::cout << dual.get_status() << std::endl;
    std::cout << dual.get_best_obj() << std::endl;

    return 0;
}
