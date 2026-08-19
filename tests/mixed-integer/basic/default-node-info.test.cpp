#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"

using namespace idol;

namespace {
class TestNodeInfo : public DefaultNodeInfo {
public:
    void compute(const Model& t_original_model) {
        compute_sum_of_infeasibilities(t_original_model);
    }
};
}

TEST_CASE("Default node infeasibility ignores continuous variables", "[basic][branch-and-bound][node-info]") {
    Env env;
    Model original(env);
    const auto integer = original.add_var(0., 10., Integer, 0., "integer");
    const auto binary = original.add_var(0., 1., Binary, 0., "binary");
    const auto continuous = original.add_var(0., 10., Continuous, 0., "continuous");

    PrimalPoint point;
    point.set(integer, 2.25);
    point.set(binary, 0.8);
    point.set(continuous, 3.4);

    TestNodeInfo info;
    info.set_primal_solution(std::move(point));
    info.compute(original);

    CHECK(info.sum_of_infeasibilities() == Catch::Approx(0.45));
}
