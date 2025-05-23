#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    // Instance data
    const unsigned int n_items = 5;
    const std::vector<double> profit { 40., 50., 100., 95., 30., };
    const std::vector<double> weight { 2., 3.14, 1.98, 5., 3., };
    const double capacity = 10.;

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Create decision variables
    auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, 0., "x");

    // Create the knapsack constraint
    model.add_ctr(idol_Sum(j, Range(n_items), weight[j] * x[j]) <= capacity);

    // Set the objective function
    model.set_obj_expr(idol_Sum(j, Range(n_items), -profit[j] * x[j]));

    // Use HIGHS as the underlying MIP solver
    model.use(HiGHS());

    // Solve the problem
    model.optimize();

    // Analyze the solution status
    const auto status = model.get_status();
    const auto reason = model.get_reason();

    std::cout << "Solution status: " << status << std::endl;
    std::cout << "Reason: " << reason << std::endl;

    if (status == Optimal || status == Feasible) {

        // Get the number of solutions in the solution pool
        const auto n_solutions = model.get_n_solutions();

        std::cout << "Number of solutions: " << n_solutions << std::endl;

        // Print all solutions in the pool
        for (unsigned int i = 0 ; i < n_solutions ; ++i) {
            model.set_solution_index(i);
            std::cout << "Solution " << i << std::endl;
            std::cout << save_primal(model) << std::endl;
        }

    }

    return 0;
}
