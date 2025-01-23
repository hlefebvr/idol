//
// Created by henri on 27.10.23.
//

#ifndef IDOL_EXPORTBRANCHANDBOUNDTREETOCSV_H
#define IDOL_EXPORTBRANCHANDBOUNDTREETOCSV_H


#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"
#include <fstream>

namespace idol::Utils {
    template<class NodeInfoT>
    class ExportBranchAndBoundTreeToCSV;
}

template<class NodeInfoT = idol::DefaultNodeInfo>
class idol::Utils::ExportBranchAndBoundTreeToCSV : public BranchAndBoundCallbackFactory<NodeInfoT> {
    const std::string m_filename;
public:
    explicit ExportBranchAndBoundTreeToCSV(std::string t_filename);

    BranchAndBoundCallback<NodeInfoT> *operator()() override;

    BranchAndBoundCallbackFactory<NodeInfoT> *clone() const override;

    class Strategy : public BranchAndBoundCallback<NodeInfoT> {
        const std::string m_filename;
    protected:
        void operator()(CallbackEvent t_event) override;
    public:
        explicit Strategy(std::string t_filename);
    };
};

template<class NodeInfoT>
idol::Utils::ExportBranchAndBoundTreeToCSV<NodeInfoT>::ExportBranchAndBoundTreeToCSV(std::string t_filename) : m_filename(std::move(t_filename)) {

}

template<class NodeInfoT>
idol::BranchAndBoundCallbackFactory<NodeInfoT>*
idol::Utils::ExportBranchAndBoundTreeToCSV<NodeInfoT>::clone() const {
    return new ExportBranchAndBoundTreeToCSV(*this);
}

template<class NodeInfoT>
idol::BranchAndBoundCallback<NodeInfoT>*
idol::Utils::ExportBranchAndBoundTreeToCSV<NodeInfoT>::operator()() {
    return new Strategy(m_filename);
}

template<class NodeInfoT>
idol::Utils::ExportBranchAndBoundTreeToCSV<NodeInfoT>::Strategy::Strategy(std::string t_filename) : m_filename(std::move(t_filename)) {

    std::ofstream file(m_filename);

    if (!file.is_open()) {
        throw Exception("Could not open destination file.");
    }

    file.close();

}

template<class NodeInfoT>
void idol::Utils::ExportBranchAndBoundTreeToCSV<NodeInfoT>::Strategy::operator()(idol::CallbackEvent t_event) {

    if (t_event != InvalidSolution && t_event != IncumbentSolution && t_event != PrunedSolution) {
        return;
    }

    const auto& side_effect_registry = this->side_effect_registry();

    if (side_effect_registry.n_added_lazy_cuts > 0 || side_effect_registry.n_added_user_cuts > 0) {
        return;
    }

    std::ofstream file(m_filename, std::ios::app);

    if (!file.is_open()) {
        throw Exception("Could not open destination file.");
    }

    const auto& node = this->node();

    const unsigned int id = node.id();
    const auto status = node.info().status();
    unsigned int parent_id;
    std::stringstream branch_label;
    double sum_of_infeasibilities = Inf;

    if (id == 0) {
        parent_id = 0;
    } else {
        parent_id = node.parent().id();

        for (const auto& branching_decision : node.info().variable_branching_decisions()) {
            branch_label << branching_decision.variable.name() << " ";
            if (branching_decision.type == LessOrEqual) {
                branch_label << "&le;";
            } else {
                branch_label << "&ge;";
            }
            branch_label << " " << branching_decision.bound;
        }

        for (const auto& branching_decisions : node.info().constraint_branching_decisions()) {
            branch_label << branching_decisions.temporary_constraint;
        }
    }

    if (status == Optimal || status == Feasible) {
        sum_of_infeasibilities = node.info().sum_of_infeasibilities();
    }

    file << this->time().count() << ","
         << id << ","
         << parent_id << ","
         << node.level() << ","
         << status << ","
         << node.info().objective_value() << ","
         << branch_label.str() << ","
         << t_event << ","
         << sum_of_infeasibilities
         << std::endl;

    file.close();

}

#endif //IDOL_EXPORTBRANCHANDBOUNDTREETOCSV_H
