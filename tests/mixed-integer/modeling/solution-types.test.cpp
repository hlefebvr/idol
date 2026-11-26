#include <catch2/catch_all.hpp>

#include <sstream>

#include "idol/general/utils/types.h"

using namespace idol;

TEST_CASE("Solution status values and output follow the public contract", "[solution-types]") {
    CHECK(static_cast<int>(Loaded) == 0);
    CHECK(static_cast<int>(Optimal) == 1);
    CHECK(static_cast<int>(Feasible) == 2);
    CHECK(static_cast<int>(Infeasible) == 3);
    CHECK(static_cast<int>(InfOrUnbnd) == 4);
    CHECK(static_cast<int>(Unbounded) == 5);
    CHECK(static_cast<int>(Unknown) == 6);

    std::ostringstream output;
    output << Unknown << ' ' << NodeLimit << ' ' << Interrupted;
    CHECK(output.str() == "Unknown NodeLimit Interrupted");
}

TEST_CASE("Dual status conversion makes no unsupported feasibility inference", "[solution-types]") {
    CHECK(dual(Loaded) == Loaded);
    CHECK(dual(Optimal) == Optimal);
    CHECK(dual(Feasible) == Unknown);
    CHECK(dual(Unknown) == Unknown);
    CHECK(dual(Infeasible) == Unbounded);
    CHECK(dual(Unbounded) == Infeasible);
    CHECK(dual(InfOrUnbnd) == InfOrUnbnd);
}
