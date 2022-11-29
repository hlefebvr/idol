#include <iostream>
#include "algorithms/parameters/Log.h"
#include "modeling.h"
#include "modeling/objects/ObjectStore.h"
#include "modeling/variables/VarAttributes.h"
#include "algorithms/solvers/Solvers_GLPK.h"
#include "problems/GAP/GAP_Instance.h"

using namespace Problems::GAP;

int main() {

    const auto instance = read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/gap_a/a05100");

    const unsigned int n_knapsacks = instance.n_knapsacks();
    const unsigned int n_items = instance.n_items();

    Model model;

    auto x = model.add_vars(Dim<2>(n_knapsacks, n_items), 0., 1., Binary, 0.);

    for (unsigned int k = 0 ; k < n_knapsacks ; ++k) {
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            model.set(Attr::Var::Obj, x[k][j], instance.p(k, j));
        }

        model.add_ctr(idol_Sum(j, Range(n_items), instance.w(k,j) * x[k][j]) <= instance.t(k));
    }

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        model.add_ctr(idol_Sum(k, Range(n_knapsacks), x[k][j]) == 1);
    }

    Solvers::GLPK solver(model);
    solver.solve();

    std::cout << solver.primal_solution().objective_value() << std::endl;
}
