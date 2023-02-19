#include <iostream>
#include "modeling.h"
#include "problems/facility-location-problem/FLP_Instance.h"
#include "backends/solvers/Gurobi.h"
#include "backends/branch-and-bound/BranchAndBound.h"
#include "backends/branch-and-bound/NodeStrategies_Basic.h"
#include "backends/branch-and-bound/Nodes_Basic.h"
#include "backends/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "backends/branch-and-bound/ActiveNodesManagers_Basic.h"
#include "backends/branch-and-bound/NodeUpdators_ByBoundVar.h"
#include "backends/branch-and-bound/Relaxations_Continuous.h"
#include "backends/BranchAndBoundMIP.h"
#include "problems/generalized-assignment-problem/GAP_Instance.h"
#include "backends/column-generation/Relaxations_DantzigWolfe.h"
#include "backends/column-generation/ColumnGeneration.h"
#include "backends/BranchAndPriceMIP.h"
#include "backends/solvers/GLPK.h"

template<class BackendT>
class IntegerMaster : public Callback {
public:
    void execute(Event t_event) override {

        if (t_event != Event::NodeSolved) {
            return;
        }

        const auto& node = node_model();

        const auto& column_generation = node.backend().template as<ColumnGeneration>();

        auto integer_master = std::unique_ptr<AbstractModel>(column_generation.master().clone());

        for (const auto& subproblem : column_generation.subproblems()) {
            for (const auto &[alpha, generator]: subproblem.present_generators()) {
                integer_master->set(Attr::Var::Type, alpha, Binary);
            }
        }

        Idol::set_optimizer<BackendT>(*integer_master);

        integer_master->optimize();

        auto solution = save(*integer_master, Attr::Solution::Primal);

        submit(std::move(solution));

    }
};

int main(int t_argc, char** t_argv) {

    Logs::set_level<BranchAndBound>(Info);
    Logs::set_color<BranchAndBound>(Blue);

    Logs::set_level<ColumnGeneration>(Info);
    Logs::set_color<ColumnGeneration>(Yellow);

    using namespace Problems::GAP;

    const auto instance = read_instance("/home/henri/CLionProjects/optimize/tests/instances/generalized-assignment-problem/GAP_instance0.txt");
    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    Env env;

    Annotation<Ctr> decomposition(env, "by_machines", MasterId);
    Annotation<Ctr> decomposition2(env, "by_machines", MasterId);

    Model model(env);

    auto x = Var::array(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");
    model.add_array<Var, 2>(x);

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        capacity.set(decomposition, i);
        capacity.set(decomposition2, 0);
        model.add(capacity);
    }

    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
        model.add(assignment);
    }

    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    auto& optimizer = Idol::set_optimizer<BranchAndPriceMIP<Gurobi>>(model, decomposition);
    optimizer.set_callback<IntegerMaster<Gurobi>>();

    model.set(Param::ColumnGeneration::LogFrequency, 1);
    model.set(Param::ColumnGeneration::FarkasPricing, true);
    model.set(Param::ColumnGeneration::BranchingOnMaster, false);
    model.set(Param::ColumnGeneration::SmoothingFactor, 0);

    //auto& optimizer = Idol::set_optimizer<BranchAndBoundMIP<GLPK>>(model);
    //optimizer.set_callback<MyCallback>();

    model.optimize();

    std::cout << save(model, Attr::Solution::Primal) << std::endl;

    return 0;
}
