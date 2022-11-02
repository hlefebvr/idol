//
// Created by henri on 07/10/22.
//

#include "../test_utils.h"
#include "algorithms/row-generation/RowGeneration.h"


TEMPLATE_LIST_TEST_CASE("13. RG: Toy", has_lp_solver ? "[generation-strategies][algorithms]" : "[.]", lp_solvers) {

    Model model;

    SECTION("Basic problem") {


        Model sp;
        auto w_1 = sp.add_var(0., Inf, Continuous, 0., "w_1");
        auto w_2 = sp.add_var(0., Inf, Continuous, 0., "w_2");
        sp.add_ctr(w_1 + 2 * w_2 <= 2.);
        sp.add_ctr(2 * w_1 + -1 * w_2 <= 3.);

        Model rmp;
        auto z = rmp.add_var(0., Inf, Continuous, 1., "z");
        auto y = rmp.add_var(0., Inf, Continuous, 2., "y");
        auto ctr = rmp.add_ctr(z + (!w_1 + 3. * !w_2) * y >= 3. * !w_1 + 4. * !w_2);

        Decomposition decomposition;
        decomposition.set_rmp_solution_strategy<TestType>(rmp);
        auto& cut_generation = decomposition.add_generation_strategy<RowGeneration>();
        auto &subproblem = cut_generation.add_subproblem(ctr);
        subproblem.set_solution_strategy<TestType>(sp);

        decomposition.solve();

        const auto solution = decomposition.primal_solution();
        CHECK(solution.status() == Optimal);
        CHECK(solution.objective_value() == 5.2857142857_a);

    }

}