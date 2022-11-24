//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_ALGORITHMS_H
#define OPTIMIZE_ALGORITHMS_H

#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/branch-and-bound/Nodes_Basic.h"
#include "algorithms/branch-and-bound/NodeStrategies_Basic.h"
#include "algorithms/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeUpdators_ByBoundVar.h"
#include "algorithms/branch-and-bound/NodeUpdators_ByBoundCtr.h"
#include "solvers.h"
#include "algorithms/decomposition/Decomposition.h"
#include "algorithms/column-generation/ColumnGeneration.h"
#include "algorithms/column-generation/ColumnGenerationBranchingSchemes_RMP.h"
#include "algorithms/branch-and-bound/ActiveNodesManagers_Basic.h"

template<
        class SolutionStrategyT = std::tuple_element_t<1, lp_solvers>,
        class BranchingStrategyT = BranchingStrategies::MostInfeasible,
        class NodeStrategyT = NodeStrategies::Basic<Nodes::Basic>,
        class ActiveNodeManagerT = ActiveNodesManagers::Basic,
        class NodeUpdatorT = NodeUpdators::ByBoundCtr
>
BranchAndBound branch_and_bound(Model& t_model, std::vector<Var> t_branching_candidates) {
    BranchAndBound result;

    auto& node_strategy = result.set_node_strategy<NodeStrategyT>();
    node_strategy.template set_active_node_manager_strategy<ActiveNodeManagerT>();
    node_strategy.template set_branching_strategy<BranchingStrategyT>(std::move(t_branching_candidates));
    node_strategy.template set_node_updator_strategy<NodeUpdatorT>();

    result.set_solution_strategy<SolutionStrategyT>(t_model);

    return result;
}

template<
        class RMPSolutionStrategyT = default_solver,
        class SPSolutionStrategyT = default_solver,
        class GenerationStrategyT = ColumnGenerationBranchingSchemes::RMP,
        class BranchingStrategyT = BranchingStrategies::MostInfeasible,
        class NodeStrategyT = NodeStrategies::Basic<Nodes::Basic>,
        class ActiveNodeManagerT = ActiveNodesManagers::Basic,
        class NodeUpdatorT = NodeUpdators::ByBoundVar
>
BranchAndBound branch_and_price(Model& t_rmp_model, const std::vector<Var>& t_variables, std::vector<Model>& t_subproblems, std::vector<Var> t_branching_candidates) {
    BranchAndBound result;

    auto& node_strategy = result.set_node_strategy<NodeStrategyT>();
    node_strategy.template set_active_node_manager_strategy<ActiveNodeManagerT>();
    node_strategy.template set_branching_strategy<BranchingStrategyT>(std::move(t_branching_candidates));
    node_strategy.template set_node_updator_strategy<NodeUpdatorT>();

    auto& decomposition = result.set_solution_strategy<Decomposition>();
    decomposition.template set_rmp_solution_strategy<RMPSolutionStrategyT>(t_rmp_model);

    auto& column_generation = decomposition.template add_generation_strategy<ColumnGeneration>();

    const unsigned int n_subproblems = t_variables.size();
    if (n_subproblems != t_subproblems.size()) { throw Exception("Subproblems and Variables size do not match."); }

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        auto &subproblem = column_generation.add_subproblem(t_variables[i]);
        subproblem.template set_solution_strategy<SPSolutionStrategyT>(t_subproblems[i]);
        subproblem.template set_branching_scheme<GenerationStrategyT>();
    }

    return result;
}

#endif //OPTIMIZE_ALGORITHMS_H
