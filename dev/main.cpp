#include "idol/modeling.h"
#include "idol/problems/knapsack-problem/KP_Instance.h"
#include "idol/modeling/models/dualize.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"

using namespace idol;

int main(int t_argc, char** t_argv) {

    const auto instance = Problems::KP::read_instance("/home/henri/Research/idol/examples/knapsack.data.txt");

    const auto n_items = instance.n_items();

    Env env;

    auto model = read_mps_file(env, "/home/henri/Downloads/SMALL/knapsack.mps");

    model.set_obj_sense(Maximize);

    model.use(Gurobi::ContinuousRelaxation());

    model.optimize();

    std::cout << "Primal: " << model.get_best_obj() << std::endl;

    auto dual = dualize(model);

    dual.use(Gurobi());

    dual.optimize();

    std::cout << "Dual: " << dual.get_best_obj() << std::endl;

    return 0;
}
