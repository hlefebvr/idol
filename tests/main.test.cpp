//
// Created by henri on 22/09/22.
//
#include "./test_utils.h"

int main( int argc, char* argv[] ) {

    Logs::set_level<BranchAndBound>(Info);
    Logs::set_level<ColumnGeneration>(Info);

    int result = Catch::Session().run( argc, argv );

    return result;
}
