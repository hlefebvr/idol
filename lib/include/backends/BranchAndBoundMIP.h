//
// Created by henri on 06/02/23.
//

#ifndef IDOL_BRANCHANDBOUNDMIP_H
#define IDOL_BRANCHANDBOUNDMIP_H

#include "backends/branch-and-bound/BranchAndBound.h"
#include "backends/branch-and-bound/Relaxations_Continuous.h"
#include "backends/branch-and-bound/NodeStrategies_Basic.h"
#include "backends/branch-and-bound/ActiveNodesManagers_Basic.h"
#include "backends/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "backends/branch-and-bound/NodeUpdators_ByBoundVar.h"
#include "backends/branch-and-bound/Nodes_Basic.h"

template<class RelaxationBackendT>
class BranchAndBoundMIP : public BranchAndBound {
public:
    explicit BranchAndBoundMIP(const Model& t_model) : BranchAndBound(t_model) {

        auto& relaxation = set_relaxation<Relaxations::Continuous>();
        relaxation.build();
        relaxation.model().template set_backend<RelaxationBackendT>();

        auto& nodes_manager = set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
        nodes_manager.template set_active_node_manager<ActiveNodesManagers::Basic>();
        nodes_manager.template set_branching_strategy<BranchingStrategies::MostInfeasible>(relaxation.branching_candidates());
        nodes_manager.template set_node_updator<NodeUpdators::ByBoundVar>();

    }
};

#endif //IDOL_BRANCHANDBOUNDMIP_H
