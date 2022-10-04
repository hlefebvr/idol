#include <iostream>
#include "modeling.h"
#include "algorithms/logs/Log.h"
#include "modeling/expressions/Expr.h"


int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    Env env;
    Model model(env);
    auto x = model.add_variable(0., 1., Integer, 0.);

    Expr expr(x);

    for (const auto& [var, coeff] : expr) {
        std::cout << var << " * " << coeff << std::endl;
    }

    return 0;
}

