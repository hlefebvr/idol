#include <iostream>
#include "idol/general/utils/SparseVector.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;
    Model model(env, Model::Storage::RowOriented);
    const auto x = model.add_vars(Dim<1>(10), 0., 1., Continuous, "x");

    Column col;
    col.set_obj(1.);
    model.add_var(0, 10, Continuous, col);

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

    return 0;
}
