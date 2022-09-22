//
// Created by henri on 22/09/22.
//
#include "./test_utils.h"

int main( int argc, char* argv[] ) {

    Log::set_level(Info);

    int result = Catch::Session().run( argc, argv );

    return result;
}