#include <catch2/catch_test_macros.hpp>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/OsiIdolSolverInterface.h"

using namespace idol;

TEST_CASE("OSI accepts minimization and rejects maximization", "[basic][osi][objective-sense]") {
    Env env;
    Model model(env);
    OsiIdolSolverInterface osi(model);

    CHECK(osi.getObjSense() == 1.);
    CHECK_NOTHROW(osi.setObjSense(1.));
    CHECK(osi.getObjSense() == 1.);
    CHECK_THROWS_AS(osi.setObjSense(-1.), Exception);
    CHECK(osi.getObjSense() == 1.);
}
