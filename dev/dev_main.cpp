#include <iostream>
#include "algorithms/logs/Log.h"
#include "modeling.h"
#include "modeling/models/ObjectStore.h"
#include "modeling/variables/VarAttributes.h"
#include "algorithms/solvers/Solvers_GLPK_Simplex.h"

int main() {

    Model model;

    auto x = model.add_vars(Dim<1>(10), 0., 1., Continuous, 1, "x");
    model.add_ctr(idol_Sum(i, Range(10), x[i]) <= 7);

    Solvers::GLPK_Simplex solver(model);
    solver.solve();

    auto c = solver.add_ctr(x[1] <= 1);

    solver.solve();

    auto c2 = solver.add_ctr(x[0] <= 2);
    solver.remove(c2);
    solver.update_rhs_coeff(c, 0);

    solver.solve();

    return 0;
}
