#include <iostream>
#include "modeling.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/column-generation/ColGenModel.h"
#include "algorithms/column-generation/ColGenerator.h"
#include "solvers/solutions/Solution.h"

int main() {

    Env env;

    {
        Model unbounded(env);

        auto x = unbounded.add_variable(0., Inf, Continuous, -3, "x");
        auto y = unbounded.add_variable(0., Inf, Continuous, -2, "y");
        auto c1 = unbounded.add_constraint(x + -2 * y <= 1);
        auto c2 = unbounded.add_constraint(-2 * x + y <= 1);
        auto c3 = unbounded.add_constraint(x + y >= 2);

        Gurobi gurobi(unbounded);
        gurobi.set_infeasible_or_unbounded_info(true);
        gurobi.set_algorithm_for_lp(PrimalSimplex);
        gurobi.solve();

        std::cout << "GUROBI UNBOUNDED" << std::endl;
        std::cout << gurobi.unbounded_ray().normalize(Inf) << std::endl;

        Lpsolve lpsolve(unbounded);
        lpsolve.set_infeasible_or_unbounded_info(true);
        lpsolve.set_algorithm_for_lp(PrimalSimplex);
        lpsolve.solve();

        std::cout << "LPSOLVE UNBOUNDED" << std::endl;
        std::cout << lpsolve.unbounded_ray().normalize(Inf) << std::endl;
    }

    {
        Model infeasible(env);

        auto u = infeasible.add_variable(0., Inf, Continuous, 1, "u");
        auto v = infeasible.add_variable(0., Inf, Continuous, 1, "v");
        auto w = infeasible.add_variable(0., Inf, Continuous, -2, "w");
        auto c1 = infeasible.add_constraint(u + -2 * v + -1 * w >= 3);
        auto c2 = infeasible.add_constraint(-2 * u + v + -1 * w >= 2);

        std::cout << "GUROBI INFEASIBLE" << std::endl;
        Gurobi gurobi(infeasible);
        gurobi.set_infeasible_or_unbounded_info(true);
        gurobi.set_algorithm_for_lp(DualSimplex);
        gurobi.solve();

        std::cout << gurobi.dual_farkas().normalize(Inf) << std::endl;

        std::cout << "LPSOLVE INFEASIBLE" << std::endl;
        Lpsolve lpsolve(infeasible);
        lpsolve.set_infeasible_or_unbounded_info(true);
        lpsolve.set_algorithm_for_lp(DualSimplex);
        lpsolve.solve();

        std::cout << lpsolve.dual_farkas().normalize(Inf) << std::endl;
    }

    return 0;
}

