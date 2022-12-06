#include <iostream>
#include "algorithms/parameters/Log.h"
#include "modeling.h"
#include "modeling/objects/ObjectStore.h"
#include "modeling/variables/VarAttributes.h"
#include "algorithms/solvers/Solvers_GLPK.h"
#include "problems/GAP/GAP_Instance.h"

using namespace Problems::GAP;

int main() {

    Model model;

    auto x = model.add_vars(Dim<1>(2), 0., 1., Binary, 0., "x");
    auto c = model.add_ctr(x[0] + x[1] <= 2);

    Expr objective = x[0];
    model.set(Attr::Obj::Expr, objective);

    auto cloned = model.clone();

    model.set(Attr::Matrix::Coeff, c, x[0], 1900);
    model.remove(x[0]);
    model.add_var(0., 9., Integer, 2, "y");

    std::cout << model << std::endl;
    std::cout << cloned << std::endl;

}
