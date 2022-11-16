#include <iostream>
#include "algorithms/logs/Log.h"
#include "algorithms/row-generation/ModelMirror.h"
#include "modeling.h"

int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);
    Log::set_color("row-generation", Color::Green);

    Model model;
    auto x = model.add_var(0., 1., Continuous, 0., "x");
    auto y = model.add_var(0., 1., Continuous, 0., "y");
    auto c = model.add_ctr(x + y <= 1);

    ModelMirror mirror(model);

    model.add_ctr(!y * x == 0);

    std::cout << mirror << std::endl;

    return 0;
}

