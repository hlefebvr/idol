#include <iostream>
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;
    Model model(env);

    const auto& x = model.add_vars(Dim<1>(5), 0, 1, Binary, -1, "x");

    model.use(
        Gurobi()
                        .with_logs(true)
                        .with_presolve(false)
                        .with_external_param("Heuristics", 0.)
                        .with_external_param("Cuts", 0)
    );
    model.optimize();

    auto& gurobi = model.optimizer().as<Optimizers::Gurobi>();
    auto& lib = Optimizers::Gurobi::get_dynamic_lib();
    auto* grb_model = gurobi.model();
    double node_count;
    lib.GRBgetdblattr(grb_model, "NodeCount", &node_count);
    std::cout << "Node count: " << node_count << std::endl;

    return 0;
}
