//
// Created by henri on 22.10.23.
//

#include <catch2/catch_all.hpp>
#include "../../cartesian_product.h"
#include "idol/modeling.h"
#include "idol/mixed-integer/problems/facility-location-problem/FLP_Instance.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Mosek/Mosek.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/modeling/models/KKT.h"

using namespace Catch::literals;
using namespace idol;

enum Reformulation {
    Dual,
    StrongDuality,
    KKT
};

std::string to_string(Reformulation reformulation) {
    switch (reformulation) {
        case Dual: return "dual";
        case StrongDuality: return "strong duality";
        case KKT: return "KKT";
        default: throw std::runtime_error("Invalid reformulation.");
    }
}

TEST_CASE("Reformulate Facility Location Problem instances using optimality conditions", "") {

    Env env;

    using namespace Problems::FLP;

    // Generate parameters
    const auto [filename, objective_value] = GENERATE(
            std::make_pair<std::string, double>("instance_F10_C20__0.txt", 210.9670332387),
            std::make_pair<std::string, double>("instance_F10_C20__1.txt", 212.5001117517),
            std::make_pair<std::string, double>("instance_F10_C20__2.txt", 220.886214295),
            std::make_pair<std::string, double>("instance_F10_C20__3.txt", 308.7378641781),
            std::make_pair<std::string, double>("instance_F10_C20__4.txt", 194.9867989575)
    );
    const auto reformulation = GENERATE(Dual, StrongDuality); /* , Reformulation::KKT */

    // Read instance
    const auto instance = read_instance_1991_Cornuejols_et_al("../../data/facility-location-problem/" + filename);
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Make model
    auto x = Var::make_vector(env, Dim<1>(n_facilities), 0., 1., Continuous, 0., "x");
    auto y = Var::make_vector(env, Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "y");

    Model model(env);

    model.add_vector<Var, 1>(x);
    model.add_vector<Var, 2>(y);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add(Ctr(env, idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]));
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add(Ctr(env, idol_Sum(i, Range(n_facilities), y[i][j]) == 1));
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * x[i] + idol_Sum(j, Range(n_customers),
                                                                                                 instance.per_unit_transportation_cost(
                                                                                                         i, j) *
                                                                                                 instance.demand(j) *
                                                                                                 y[i][j])));

    Model reformulated(env);

    Reformulators::KKT reformulator(model, model.get_obj_expr());

    switch (reformulation) {
        case Dual:
            reformulator.add_dual(reformulated);
            break;
        case StrongDuality:
            reformulator.add_strong_duality_reformulation(reformulated);
            break;
        case KKT:
            reformulator.add_kkt_reformulation(reformulated);
            break;
        default: throw std::runtime_error("Invalid reformulation.");
    }

    reformulated.use(OPTIMIZER());

    WHEN("The instance \"" + filename + "\" is solved after reformulating using " + to_string(reformulation)) {

        reformulated.optimize();

        THEN("The solution status is Optimal") {

            CHECK(reformulated.get_status() == Optimal);

        }

        AND_THEN("The objective value is " + std::to_string(objective_value)) {

            const auto primal_solution = save_primal(reformulated);
            double value = evaluate(reformulator.get_dual_obj_expr(), primal_solution);

            CHECK(value == Catch::Approx(objective_value));

            if (reformulation != Reformulation::Dual) {
                value = evaluate(model.get_obj_expr(), primal_solution);
                CHECK(value == Catch::Approx(objective_value));
            }

        }

    }

}
