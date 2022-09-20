#include <iostream>
#include "modeling.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/branch-and-bound/BranchAndBound.test.h"
#include "algorithms/logs/Log.h"
#include "algorithms/branch-and-cut-and-price/DecompositionStrategy.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeByBound.h"
#include "algorithms/branch-and-bound/BaseNodeStrategy.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerator.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerationStrategy.h"
#include "algorithms/branch-and-bound/NodeByBoundStrategy.h"
#include "algorithms/branch-and-cut-and-price/DantzigWolfeGenerator.h"

class Counter : public Listener {
public:
    unsigned int n_adds = 0;
    unsigned int n_removes = 0;
protected:
    void on_add(const Var &t_var) override {
        ++n_adds;
    }

    void on_remove(const Var &t_var) override {
        ++n_removes;
    }
};

int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    Env env;

    Model rmp(env);
    auto x_bar_0 = rmp.add_virtual_variable(0., 10., Continuous, 0., "x_bar_0");
    auto x_bar_1 = rmp.add_virtual_variable(0., 10., Continuous, 0., "x_bar_1");
    auto ctr_rmp = rmp.add_constraint(-2. * x_bar_0 + 2. * x_bar_1 >= 1., "rmp_ctr");
    //auto ctr_rmp = rmp.add_constraint(GreaterOrEqual, 1., "rmp_ctr");
    auto ctr_con = rmp.add_constraint(Equal, 1);

    Model sp(env);
    auto x_0 = sp.add_variable(0., 10., Continuous, -1., "x_0");
    auto x_1 = sp.add_variable(0., 10., Continuous, -1., "x_1");
    auto sp_ctr = sp.add_constraint(-8 * x_0 + 10. * x_1 <= 13.);

    /*ColumnGenerator generator(rmp, sp);
    generator.set(ctr_rmp, -2. * x_0 + 2. * x_1);
    generator.set(ctr_con, Expr(), 1.);*/

    DantzigWolfeGenerator generator(rmp, sp, ctr_con);
    generator.set(x_bar_0, x_0);
    generator.set(x_bar_1, x_1);

    BranchAndBound solver;
    solver.set_node_strategy<NodeByBoundStrategy>();
    solver.set_branching_strategy<MostInfeasible>(std::vector<Var> { x_bar_0, x_bar_1 });

    auto& generation_strategy = solver.set_solution_strategy<DecompositionStrategy<Gurobi>>(rmp);
    auto& column_generation = generation_strategy.add_generation_strategy<ColumnGenerationStrategy>();
    auto& subproblem  = column_generation.add_subproblem<ExternalSolverStrategy<Lpsolve>>(generator, sp);

    Counter counter;
    rmp.add_listener(counter);

    solver.solve();

    std::cout << "Status: " << solver.status() << std::endl;
    std::cout << "Optimum: " << solver.objective_value() << std::endl;
    std::cout << "N. created nodes: " << solver.n_created_nodes() << std::endl;

    std::cout << "N. adds = " << counter.n_adds << std::endl;
    std::cout << "N. removes = " << counter.n_removes << std::endl;

    return 0;
}

