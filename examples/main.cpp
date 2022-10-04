#include <iostream>
#include "modeling.h"
#include "algorithms/logs/Log.h"
#include "modeling/expressions/LinExpr.h"


int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    Env env;
    Model model(env);
    auto x = model.add_variable(0., 1., Integer, 0.);

    LinExpr map;
    map.set(x, 3);

    for (auto [var, coeff] : map) {
        std::cout << var << std::endl;
    }

    return 0;
}

