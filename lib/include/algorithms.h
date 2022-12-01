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
        class SolutionStrategyT = std::tuple_element_t<0, lp_solvers>,
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
        class NodeUpdatorT = NodeUpdators::ByBoundVar,
        class VariableIteratorT,
        class SubproblemIteratorT
>
BranchAndBound branch_and_price(
        Model& t_rmp_model,
        VariableIteratorT t_variable_begin,
        VariableIteratorT t_variable_end,
        SubproblemIteratorT t_subproblem_begin,
        SubproblemIteratorT t_subproblem_end,
        std::vector<Var> t_branching_candidates
    ) {

    BranchAndBound result;

    auto& node_strategy = result.set_node_strategy<NodeStrategyT>();
    node_strategy.template set_active_node_manager_strategy<ActiveNodeManagerT>();
    node_strategy.template set_branching_strategy<BranchingStrategyT>(std::move(t_branching_candidates));
    node_strategy.template set_node_updator_strategy<NodeUpdatorT>();

    auto& decomposition = result.set_solution_strategy<Decomposition>();
    decomposition.template set_rmp_solution_strategy<RMPSolutionStrategyT>(t_rmp_model);

    auto& column_generation = decomposition.template add_generation_strategy<ColumnGeneration>();

    for (
            ; t_variable_begin != t_variable_end && t_subproblem_begin != t_subproblem_end
            ; ++t_variable_begin, ++t_subproblem_begin) {

        auto &subproblem = column_generation.add_subproblem(*t_variable_begin);
        subproblem.template set_solution_strategy<SPSolutionStrategyT>(*t_subproblem_begin);
        subproblem.template set_branching_scheme<GenerationStrategyT>();

    }

    if (t_variable_begin != t_variable_end || t_subproblem_begin != t_subproblem_end) {
        throw Exception("Variable and subproblem size do not match.");
    }

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
BranchAndBound branch_and_price(
        Model& t_rmp_model,
        const Var& t_variable,
        Model& t_subproblem,
        std::vector<Var> t_branching_candidates
) {

    std::initializer_list<Var> variables { t_variable };

    class iterator {
        bool m_is_end;
        Model& m_model;
    public:
        iterator(Model& t_model, bool t_is_end) : m_is_end(t_is_end), m_model(t_model) {}
        iterator& operator++() { m_is_end = m_is_end + 1; return *this; }
        Model& operator*() { return m_model; }
        bool operator!=(const iterator& t_rhs) const { return m_is_end != t_rhs.m_is_end; }
    };

    return branch_and_price<
            RMPSolutionStrategyT,
            SPSolutionStrategyT,
            GenerationStrategyT,
            BranchingStrategyT,
            NodeStrategyT,
            ActiveNodeManagerT,
            NodeUpdatorT>(t_rmp_model,
                          variables.begin(),
                          variables.end(),
                          iterator(t_subproblem, false),
                          iterator(t_subproblem, true),
                          std::move(t_branching_candidates)
                          );

}

#endif //OPTIMIZE_ALGORITHMS_H
