#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Add variables
    const auto x = model.add_var(0, Inf, Continuous, 3, "x");
    const auto y = model.add_var(0, Inf, Continuous, 2, "y");

    // Add constraints
    const auto c1 = model.add_ctr(x + y <= 4);
    const auto c2 = model.add_ctr(2 * x + y <= 5);
    const auto c3 = model.add_ctr(-x + 4 * y >= 2);

    // Use GLP as the underlying LP solver
    model.use(GLPK());

    // Solve the problem
    model.optimize();

    // Analyze the solution status
    const auto status = model.get_status();
    const auto reason = model.get_reason();

    std::cout << "Solution status: " << status << std::endl;
    std::cout << "Reason: " << reason << std::endl;

    if (status == Optimal || status == Feasible) {

        std::cout << "Primal solution:\n";
        for (const auto& var : model.vars()) {
            std::cout << var.name() << " = " << model.get_var_primal(var) << std::endl;
        }

        std::cout << "Dual solution:\n";
        for (const auto& ctr : model.ctrs()) {
            std::cout << ctr.name() << " = " << model.get_ctr_dual(ctr) << std::endl;
        }

    } else if (status == Infeasible) {

        std::cout << "Farkas certificate:\n";
        for (const auto& ctr : model.ctrs()) {
            std::cout << ctr.name() << " = " << model.get_ctr_farkas(ctr) << std::endl;
        }

    } else if (status == Unbounded) {

        std::cout << "Primal ray:\n";
        for (const auto& var : model.vars()) {
            std::cout << var.name() << " = " << model.get_var_ray(var) << std::endl;
        }

    }

    return 0;
}
