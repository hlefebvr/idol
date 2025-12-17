//
// Created by henri on 17.12.25.
//
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/NodeWithCGInfo.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/Optimizers_BranchAndBound.h"

void idol::NodeWithCGInfo::save(const Model& t_original_formulation, const Model& t_model) {

    DefaultNodeInfo::save(t_original_formulation, t_model);

    if (const auto status = this->status() ; status != Optimal && status != Feasible ) {
        return;
    }

    if (!t_model.optimizer().is<Optimizers::DantzigWolfeDecomposition>()) {
        std::cout << "Oups" << std::endl;
        std::cerr << "NodeWithCGInfo was not able to save active columns.\n"
                     "Reason: the optimizer could not be cast to Optimizers::DantzigWolfeDecomposition."
                  << std::endl;
        throw Exception("STOP");
        return;
    }

    const auto& optimizer = t_model.optimizer().as<Optimizers::DantzigWolfeDecomposition>();
    const auto& formulation = optimizer.formulation();
    const unsigned int n_subproblems = formulation.n_sub_problems();

    m_active_columns.resize(n_subproblems);

    for (unsigned int k = 0 ; k < n_subproblems ; k++) {

        const auto& container = formulation.present_generators(k);
        m_active_columns.reserve(container.size());

        for (const auto& [var, col] : container) {

            const double val = t_model.get_var_primal(var);
            if (is_zero(val, Tolerance::Sparsity)) {
                continue;
            }
            m_active_columns[k].emplace_back(val, col);

        }

    }

}

idol::NodeWithCGInfo* idol::NodeWithCGInfo::clone() const {
    return new NodeWithCGInfo(*this);
}

idol::NodeWithCGInfo* idol::NodeWithCGInfo::create_child() const {
    return new NodeWithCGInfo();
}

idol::DefaultNodeUpdator<idol::NodeWithCGInfo>* idol::NodeWithCGInfo::create_updator(const Model& t_src_model,
                                                                                     Model& t_relaxation) {
    return new DefaultNodeUpdator<NodeWithCGInfo>(t_src_model, t_relaxation);
}

const idol::NodeWithCGInfo::ActiveColumns& idol::NodeWithCGInfo::get_active_columns(const Model& t_model) {

    if (!t_model.optimizer().is<Optimizers::BranchAndBound<NodeWithCGInfo>>()) {
        throw Exception("Could not retrieve active columns. Reason: the optimizer could not be cast to Optimizers::BranchAndBound<NodeWithCGInfo>");
    }

    if (const auto status = t_model.get_status() ; status != Optimal && status != Feasible) {
        throw Exception("Could not retrieve active columns. Reason: the model is not feasible.");
    }

    const auto& branch_and_bound = t_model.optimizer().as<Optimizers::BranchAndBound<NodeWithCGInfo>>();
    const auto& incumbent = branch_and_bound.incumbent().info();

    return incumbent.m_active_columns;

}

