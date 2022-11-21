#include <iostream>
#include "algorithms/logs/Log.h"
#include "algorithms/row-generation/ModelMirror.h"
#include "modeling.h"
#include "modeling/expressions/QuadExpr.h"

int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);
    Log::set_color("row-generation", Color::Green);

    Model model;
    auto x = model.add_var(0., 1., Binary, 0., "x");
    auto y = model.add_var(0., 1., Binary, 0., "y");

    QuadExpr expr;
    expr.set(x, y, 1 + !x);
    expr.set(y, x, 2);
    expr.set(x, x, 9);

    std::cout << expr.get(x, y) << std::endl;
    std::cout << expr.get(y, x) << std::endl;
    std::cout << expr.get(x, x) << std::endl;
    std::cout << expr.get(y, y) << std::endl;

    for (const auto& [var1, var2, factor] : expr) {
        std::cout << var1 << " * " << var2 << " * (" << factor << ")" << std::endl;
    }

    std::cout << expr << std::endl;

    return 0;
}

