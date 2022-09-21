//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_ALGORITHMS_H
#define OPTIMIZE_ALGORITHMS_H

#include "./algorithms/branch-and-bound/BranchAndBound.h"
#include "./algorithms/branch-and-bound/nodes/NodeByBound.h"
#include "./algorithms/branch-and-bound/node-strategies/NodeStrategy.h"
#include "./algorithms/branch-and-bound/branching-strategies/MostInfeasible.h"
#include "./algorithms/branch-and-bound/node-updators/NodeUpdatorByBound.h"
#include "./algorithms/solution-strategies/external-solver/ExternalSolverStrategy.h"
#include "./solvers.h"
#include "algorithms/solution-strategies/decomposition/DecompositionStrategy.h"
#include "algorithms/solution-strategies/column-generation/ColumnGenerationStrategy.h"
#include "algorithms/solution-strategies/column-generation/generators/DantzigWolfe_RMP_Strategy.h"

template<
        class SolutionStrategyT = ExternalSolverStrategy< std::tuple_element_t<0, available_solvers> >,
        class BranchingStrategyT = MostInfeasible,
        class NodeStrategyT = NodeStrategy<NodeByBound>,
        class ActiveNodeManagerT = ActiveNodeManager_Heap,
        class NodeUpdatorT = NodeUpdatorByBound
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
        class RMPSolutionStrategyT = ExternalSolverStrategy< std::tuple_element_t<0, available_solvers> >,
        class SPSolutionStrategyT = ExternalSolverStrategy< std::tuple_element_t<0, available_solvers> >,
        class GenerationStrategyT = DantzigWolfe_RMP_Strategy,
        class BranchingStrategyT = MostInfeasible,
        class NodeStrategyT = NodeStrategy<NodeByBound>,
        class ActiveNodeManagerT = ActiveNodeManager_Heap,
        class NodeUpdatorT = NodeUpdatorByBound,
        class IteratorT
>
BranchAndBound branch_and_price(Model& t_rmp_model, IteratorT t_begin, IteratorT t_end, std::vector<Var> t_branching_candidates) {
    BranchAndBound result;

    auto& node_strategy = result.set_node_strategy<NodeStrategyT>();
    node_strategy.template set_active_node_manager_strategy<ActiveNodeManagerT>();
    node_strategy.template set_branching_strategy<BranchingStrategyT>(std::move(t_branching_candidates));
    node_strategy.template set_node_updator_strategy<NodeUpdatorT>();

    auto& decomposition = result.set_solution_strategy<DecompositionStrategy>();
    decomposition.template set_rmp_solution_strategy<RMPSolutionStrategyT>(t_rmp_model);

    auto& column_generation = decomposition.template add_generation_strategy<ColumnGenerationStrategy>();

    for (; t_begin != t_end ; ++t_begin) {
        auto &subproblem = column_generation.template add_subproblem();
        subproblem.template set_solution_strategy<SPSolutionStrategyT>(*t_begin);
        subproblem.template set_generation_strategy<GenerationStrategyT>(t_rmp_model, *t_begin);
    }

    return result;
}

template<
    class RMPSolutionStrategyT = ExternalSolverStrategy< std::tuple_element_t<0, available_solvers> >,
    class SPSolutionStrategyT = ExternalSolverStrategy< std::tuple_element_t<0, available_solvers> >,
    class GenerationStrategyT = DantzigWolfe_RMP_Strategy,
    class BranchingStrategyT = MostInfeasible,
    class NodeStrategyT = NodeStrategy<NodeByBound>,
    class ActiveNodeManagerT = ActiveNodeManager_Heap,
    class NodeUpdatorT = NodeUpdatorByBound
>
BranchAndBound branch_and_price(Model& t_rmp_model, Model& t_subproblem, std::vector<Var> t_branching_candidates) {

    class iterator {
        bool m_is_begin;
        Model* m_model;
    public:
        iterator(Model& t_model, bool t_is_begin) : m_is_begin(t_is_begin), m_model(&t_model) {}
        bool operator!=(const iterator& t_rhs) const { return m_is_begin != t_rhs.m_is_begin; }
        iterator& operator++() { m_is_begin = false; return *this; }
        Model& operator*() { return *m_model; }
    };

    return branch_and_price<
            RMPSolutionStrategyT,
            SPSolutionStrategyT,
            GenerationStrategyT,
            BranchingStrategyT,
            NodeStrategyT,
            ActiveNodeManagerT,
            NodeUpdatorT
        >(t_rmp_model, iterator(t_subproblem, true), iterator(t_subproblem, false), std::move(t_branching_candidates));
}

#endif //OPTIMIZE_ALGORITHMS_H
