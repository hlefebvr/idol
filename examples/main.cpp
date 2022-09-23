#include <iostream>
#include "modeling.h"
#include "algorithms/logs/Log.h"


int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    return 0;
}

