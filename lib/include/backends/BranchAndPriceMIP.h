//
// Created by henri on 07/02/23.
//

#ifndef IDOL_BRANCHANDPRICEMIP_H
#define IDOL_BRANCHANDPRICEMIP_H

#include "backends/branch-and-bound/BranchAndBound.h"
#include "backends/column-generation/Relaxations_DantzigWolfe.h"
#include "backends/branch-and-bound/NodeStrategies_Basic.h"
#include "backends/branch-and-bound/ActiveNodesManagers_Basic.h"
#include "backends/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "backends/branch-and-bound/NodeUpdators_ByBoundVar.h"
#include "backends/branch-and-bound/Nodes_Basic.h"
#include "backends/column-generation/ColumnGeneration.h"

template<class RelaxationBackendT>
class BranchAndPriceMIP : public BranchAndBound {
public:
    explicit BranchAndPriceMIP(const AbstractModel& t_original_formulation,
                               const Annotation<Ctr, unsigned int>& t_annotation)
        : BranchAndBound(t_original_formulation) {

        auto& relaxation = set_relaxation<Relaxations::DantzigWolfe>(t_annotation);
        relaxation.build();

        auto& column_generation = Idol::set_optimizer<ColumnGeneration>(relaxation.model());

        column_generation.template set_master_backend<RelaxationBackendT>();
        for (unsigned int i = 0, n = relaxation.model().n_blocks() ; i < n ; ++i) {
            column_generation.template set_subproblem_backend<RelaxationBackendT>(i);
        }

        auto& nodes_manager = set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
        nodes_manager.template set_active_node_manager<ActiveNodesManagers::Basic>();
        nodes_manager.template set_branching_strategy<BranchingStrategies::MostInfeasible>(relaxation.branching_candidates());
        nodes_manager.template set_node_updator<NodeUpdators::ByBoundVar>();

    }
};

#endif //IDOL_BRANCHANDPRICEMIP_H
