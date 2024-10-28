#include <iostream>
#include "idol/general/utils/SparseVector.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include <gurobi_c++.h>

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    Var x(env, 0, Inf, Continuous, 0., "x");
    Var y(env, 0, Inf, Continuous, 0., "y");

    Ctr c1(env, 120 * x + 210 * y <= 15000);
    Ctr c2(env, 110 * x +  30 * y <=  4000);
    Ctr c3(env,       x +       y <=    75);

    Model model(env);
    model.add(x);
    model.add(y);
    model.add(c1);
    model.add(c2);
    model.add(c3);
    model.set_obj_expr(-143 * x - 60 * y);

    model.use(Gurobi().with_logs(true).with_infeasible_or_unbounded_info(true).with_presolve(false));

    model.write("model.lp");

    model.optimize();

    std::cout << model.get_status() << std::endl;

    const auto dual_solution = save_dual(model);


    /*

    Env env;
    Model model(env, Model::Storage::RowOriented);
    const auto x = model.add_vars(Dim<1>(10), 0., 1., Continuous, 1, "x");

    LinExpr<Ctr> col;
    model.add_var(0, 10, Continuous, 1, col);

    for (unsigned int i = 0; i < 9; ++i) {
        model.add_ctr(x[i] + x[i+1] >= 0.5);
    }

    //std::cout << model << std::endl;

    const auto column = model.get_var_column(x[3]);

    model.dump();

    model.set_storage(Model::Storage::ColumnOriented, true);

    const auto& row = model.get_ctr_row(model.get_ctr_by_index(2));

    model.dump();

    model.set_storage(Model::Storage::Both);

    model.dump();
     */

    return 0;
}
