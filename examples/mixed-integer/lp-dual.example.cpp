#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    /***************************/
    /* Create the primal model */
    /***************************/

    Model primal(env);

    // Add primal variables
    const auto x = primal.add_var(0, Inf, Continuous, 3, "x");
    const auto y = primal.add_var(0, Inf, Continuous, 2, "y");

    // Add primal constraints
    const auto c1 = primal.add_ctr(x + y <= 4);
    const auto c2 = primal.add_ctr(2 * x + y <= 5);
    const auto c3 = primal.add_ctr(-x + 4 * y >= 2);

    /*************************/
    /* Create the dual model */
    /*************************/

    Model dual(env);

    // Create a KKT reformulator
    // This object can generate various reformulations of the problem such as its dual problem or its KKT conditions.
    Reformulators::KKT reformulator(primal);

    // Add the dual problem to the model
    reformulator.add_dual(dual);

    // Print both the primal and the dual models
    std::cout << "Primal model:\n" << primal << std::endl;
    std::cout << "Dual model:\n" << dual << std::endl;

    /**************************/
    /* Solve the dual problem */
    /**************************/

    dual.use(HiGHS());
    dual.optimize();

    std::cout << "Dual solution status: " << dual.get_status() << std::endl;
    std::cout << "Dual objective value: " << dual.get_best_obj() << std::endl;

    // Get the dual value of each primal constraint
    for (const auto& ctr : primal.ctrs()) {
        const auto& dual_var = reformulator.get_dual_var(ctr);
        std::cout << ctr.name() << " with dual variable " << dual_var.name() << " = " << dual.get_var_primal(dual_var) << std::endl;
    }

    // Get the dual value of each primal variable's lower bound constraint
    for (const auto& var : primal.vars()) {
        const auto& dual_var = reformulator.get_dual_var_lb(var);
        std::cout << var.name() << " with dual variable for its lower bound " << dual_var.name() << " = " << dual.get_var_primal(dual_var) << std::endl;
    }

    // Primal variables having no upper bound, they do not have a dual variable for their upper bound

    return 0;
}
