#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Add variables
    const auto x = model.add_vars(Dim<1>(3), 0, 1, Continuous, 0, "x");

    // Add SOS1 constraint
    model.add_sosctr(true, x, { 1, 2, 3 }); // First argument is true if this SOS constraint is of type 1, otherwise, it is of type 2

    // Set objective function
    model.set_obj_expr(-2 * x[0] - x[1] - x[2]);

    // Use Gurobi and optimize
    model.use(Gurobi());
    model.optimize();

    if (model.get_status() == Optimal) {
        std::cout << "Optimal solution found!" << std::endl;
        std::cout << save_primal(model) << std::endl;
    }

    return 0;
}
