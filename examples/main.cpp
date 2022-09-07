#include <iostream>
#include "modeling.h"
#include "modeling/ColumnOrRow.h"

int main() {

    Env env;

    Model model(env);

    Variable<Decision> x = model.add_variable(0., Inf, Continuous, "x");
    auto y = model.add_variable(0., Inf, Continuous, "y");
    auto xi = model.add_parameter(0., Inf, Continuous, "xi");
    auto ctr = model.add_constraint(x <= 1);

    ColumnOrRow<Variable<Decision>, Decision> row;
    ColumnOrRow<Constraint<Decision>, Decision> col;

    auto ref = row.set(x, 2 * xi);
    col.set(ctr, ref);

    std::cout << row.get(x) << std::endl;
    std::cout << col.get(ctr) << std::endl;

    col.set(ctr, 7);

    std::cout << row.get(x) << std::endl;
    std::cout << col.get(ctr) << std::endl;

    row.set(x, 1000);

    std::cout << row.get(x) << std::endl;
    std::cout << col.get(ctr) << std::endl;

    auto ref_c = row.set_constant(10);
    col.set_constant(ref_c);

    std::cout << row.get_constant() << std::endl;
    std::cout << col.get_constant() << std::endl;

    col.set_constant(999);

    std::cout << row.get_constant() << std::endl;
    std::cout << col.get_constant() << std::endl;

    std::cout << "LOOP" << std::endl;

    for (auto [var, coeff] : (const ColumnOrRow<Variable<Decision>, Decision>&) row) {
        std::cout << var << " * " << coeff << std::endl;
    }

    return 0;
}
