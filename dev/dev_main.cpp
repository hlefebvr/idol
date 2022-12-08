#include <iostream>
#include "algorithms/parameters/Log.h"
#include "modeling.h"
#include "modeling/objects/ObjectStore.h"
#include "modeling/variables/VarAttributes.h"
#include "algorithms/solvers/Solvers_GLPK.h"
#include "problems/GAP/GAP_Instance.h"

using namespace Problems::GAP;

void f(const Attribute& t_attr) {
    if (t_attr == Attr::Var::Lb) {
        std::cout << "OK" << std::endl;
    }
}

int main() {

    f(Attr::Var::Lb);
    f(Attr::Var::Ub);

    Model model;

    auto x = model.add_var(0., 1., Binary, 0);

    model.get(Attr::Var::Lb, x);

}
