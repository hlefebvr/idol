//
// Created by henri on 22/09/22.
//
#include "./test_utils.h"
#include "optimizers/parameters/Logs.h"

int main( int argc, char* argv[] ) {

    int result = Catch::Session().run( argc, argv );

    return result;
}
