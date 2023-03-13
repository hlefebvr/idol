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
#include "backends/column-generation/Callbacks_IntegerMaster.h"
#include "backends/branch-and-bound/Relaxations_Continuous.h"

IDOL_CREATE_PARAMETER_CLASS(BranchAndPrice);

IDOL_CREATE_PARAMETER_TYPE(BranchAndPrice, bool, 1)

IDOL_CREATE_PARAMETER(BranchAndPrice, bool, 0, IntegerMasterHeuristic, true);

template<class MasterProblemBackendT, class SubProblemBackendT = MasterProblemBackendT>
class BranchAndPriceMIP : public BranchAndBound {
    Param::BranchAndPrice::values<bool> m_bool_params;

    void configure_column_generation(BlockModel<Ctr>& t_relaxation) {

        auto& column_generation = Idol::set_optimizer<ColumnGeneration>(t_relaxation);

        column_generation.template set_master_optimizer<MasterProblemBackendT>();
        for (unsigned int i = 0, n = t_relaxation.n_blocks() ; i < n ; ++i) {
            column_generation.template set_subproblem_backend<SubProblemBackendT>(i);
        }

    }

    void configure_branching(std::list<Var> t_branching_candidates) {

        auto& nodes_manager = set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
        nodes_manager.template set_active_node_manager<ActiveNodesManagers::Basic>();
        nodes_manager.template set_branching_strategy<BranchingStrategies::MostInfeasible>(std::move(t_branching_candidates));
        nodes_manager.template set_node_updator<NodeUpdators::ByBoundVar>();

    }
protected:
    void hook_before_optimize() override {
        BranchAndBound::hook_before_optimize();

        if (get(Param::BranchAndPrice::IntegerMasterHeuristic)) {
            add_callback<Callbacks::BranchAndPrice::IntegerMaster<MasterProblemBackendT>>();
        }

    }

    using BranchAndBound::set;
    using BranchAndBound::get;

    void set(const Parameter<bool>& t_param, bool t_value) override;
    [[nodiscard]] bool get(const Parameter<bool>& t_param) const override;
public:
    BranchAndPriceMIP(const AbstractModel& t_original_formulation, const Annotation<Ctr, unsigned int>& t_annotation) : BranchAndBound(t_original_formulation) {

        auto& relaxation = set_relaxation<Relaxations::DantzigWolfe>(t_annotation);
        relaxation.build();

        configure_column_generation(relaxation.model());
        configure_branching(relaxation.branching_candidates());
    }

    explicit BranchAndPriceMIP(const BlockModel<Ctr>& t_block_model) : BranchAndBound(t_block_model) {

        auto& relaxation = set_relaxation<Relaxations::Continuous>();
        relaxation.build();

        auto& model = relaxation.model().template as<BlockModel<Ctr>>();

        configure_column_generation(model);
        configure_branching(relaxation.branching_candidates());

    }
};

template<class MasterProblemBackendT, class SubProblemBackendT>
void BranchAndPriceMIP<MasterProblemBackendT, SubProblemBackendT>::set(const Parameter<bool> &t_param, bool t_value) {


    if (t_param.is_in_section(Param::Sections::BranchAndPrice)) {
        m_bool_params.set(t_param, t_value);
        return;
    }


    BranchAndBound::set(t_param, t_value);
}

template<class MasterProblemBackendT, class SubProblemBackendT>
bool BranchAndPriceMIP<MasterProblemBackendT, SubProblemBackendT>::get(const Parameter<bool> &t_param) const {

    if (t_param.is_in_section(Param::Sections::BranchAndPrice)) {
        return m_bool_params.get(t_param);
    }

    return Algorithm::get(t_param);
}

#endif //IDOL_BRANCHANDPRICEMIP_H
