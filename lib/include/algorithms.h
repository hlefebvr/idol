//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_ALGORITHMS_H
#define OPTIMIZE_ALGORITHMS_H

#include "./algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/branch-and-bound/Nodes_Basic.h"
#include "algorithms/branch-and-bound/NodeStrategies_Basic.h"
#include "algorithms/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeUpdators_ByBound.h"
#include "algorithms/branch-and-bound/ByBoundCtr.h"
#include "./algorithms/external-solver/ExternalSolver.h"
#include "./solvers.h"
#include "algorithms/decomposition/Decomposition.h"
#include "algorithms/column-generation/ColumnGeneration.h"
#include "algorithms/column-generation/ColumnGenerators_DantzigWolfeRMP.h"
#include "algorithms/branch-and-bound/ActiveNodesManagers_Heap.h"

template<
        class SolutionStrategyT = ExternalSolver< std::tuple_element_t<0, available_solvers> >,
        class BranchingStrategyT = BranchingStrategies::MostInfeasible,
        class NodeStrategyT = NodeStrategies::Basic<Nodes::Basic>,
        class ActiveNodeManagerT = ActiveNodesManagers::Heap,
        class NodeUpdatorT = NodeUpdators::ByBoundVar
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
        class RMPSolutionStrategyT = ExternalSolver< std::tuple_element_t<0, available_solvers> >,
        class SPSolutionStrategyT = ExternalSolver< std::tuple_element_t<0, available_solvers> >,
        class GenerationStrategyT = ColumnGenerators::DantzigWolfeRMP,
        class BranchingStrategyT = BranchingStrategies::MostInfeasible,
        class NodeStrategyT = NodeStrategies::Basic<Nodes::Basic>,
        class ActiveNodeManagerT = ActiveNodesManagers::Heap,
        class NodeUpdatorT = NodeUpdators::ByBoundVar,
        class IteratorT
>
BranchAndBound branch_and_price(Model& t_rmp_model, IteratorT t_begin, IteratorT t_end, std::vector<Var> t_branching_candidates) {
    BranchAndBound result;

    auto& node_strategy = result.set_node_strategy<NodeStrategyT>();
    node_strategy.template set_active_node_manager_strategy<ActiveNodeManagerT>();
    node_strategy.template set_branching_strategy<BranchingStrategyT>(std::move(t_branching_candidates));
    node_strategy.template set_node_updator_strategy<NodeUpdatorT>();

    auto& decomposition = result.set_solution_strategy<Decomposition>();
    decomposition.template set_rmp_solution_strategy<RMPSolutionStrategyT>(t_rmp_model);

    auto& column_generation = decomposition.template add_generation_strategy<ColumnGeneration>();

    for (; t_begin != t_end ; ++t_begin) {
        auto &subproblem = column_generation.add_subproblem();
        subproblem.template set_solution_strategy<SPSolutionStrategyT>(*t_begin);
        subproblem.template set_generation_strategy<GenerationStrategyT>(t_rmp_model, *t_begin);
    }

    return result;
}

template<
    class RMPSolutionStrategyT = ExternalSolver< std::tuple_element_t<0, available_solvers> >,
    class SPSolutionStrategyT = ExternalSolver< std::tuple_element_t<0, available_solvers> >,
    class GenerationStrategyT = ColumnGenerators::DantzigWolfeRMP,
    class BranchingStrategyT = BranchingStrategies::MostInfeasible,
    class NodeStrategyT = NodeStrategies::Basic<Nodes::Basic>,
    class ActiveNodeManagerT = ActiveNodesManagers::Heap,
    class NodeUpdatorT = NodeUpdators::ByBoundVar
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
