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

    auto x = model.add_vars(Dim<1>(3), 0., 1., Continuous, 0.);

    Row row;
    row.set_rhs(100);
    row.linear().set(x[0], 101);
    row.linear().set(x[1], 102);
    row.linear().set(x[2], 103);
    row.quadratic().set(x[0], x[0], 104);
    row.quadratic().set(x[0], x[1], 105);
    row.quadratic().set(x[0], x[2], 106);
    row.quadratic().set(x[1], x[0], 107);
    row.quadratic().set(x[1], x[1], 108);
    row.quadratic().set(x[1], x[2], 109);
    row.quadratic().set(x[2], x[0], 110);
    row.quadratic().set(x[2], x[1], 111);
    row.quadratic().set(x[2], x[2], 112);

    auto ctr = model.add_ctr(TempCtr(std::move(row), LessOrEqual));

    model.get(Attr::Rhs::Expr).get(ctr).numerical() == 100;
    model.get(Attr::Ctr::Rhs, ctr).numerical() == 100;
    model.get(Attr::Ctr::Row, ctr).linear().get(x[0]).numerical() == 101;
    model.get(Attr::Ctr::Row, ctr).linear().get(x[1]).numerical() == 102;
    model.get(Attr::Ctr::Row, ctr).linear().get(x[2]).numerical() == 103;

}
