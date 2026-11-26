#include <iostream>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/presets/BranchAndPrice.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

using namespace idol;

int main() {
    Env env;
    Model model(env);
    Annotation<unsigned int> decomposition(env, "decomposition", MasterId);
    const auto x = model.add_vars(Dim<2>(2, 2), 0., 1., Binary, 1., "x");

    const auto block_0 = model.add_ctr(x[0][0] + x[0][1] >= 1, "block_0");
    const auto block_1 = model.add_ctr(x[1][0] + x[1][1] >= 1, "block_1");
    block_0.set(decomposition, 0);
    block_1.set(decomposition, 1);
    model.add_ctr(x[0][0] + x[1][0] <= 1, "linking_0");
    model.add_ctr(x[0][1] + x[1][1] <= 1, "linking_1");

    model.use(Presets::BranchAndPrice(
            decomposition,
            GLPK::ContinuousRelaxation(),
            GLPK()));
    model.optimize();

    std::cout << save_primal(model) << std::endl;
}
