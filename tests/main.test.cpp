//
// Created by henri on 22/09/22.
//
#include "./test_utils.h"
#include "backends/parameters/Logs.h"

int main( int argc, char* argv[] ) {

    Logs::set_level<BranchAndBoundOptimizer<>>(Info);
    Logs::set_level<ColumnGenerationOptimizer>(Info);

    int result = Catch::Session().run( argc, argv );

    return result;
}
