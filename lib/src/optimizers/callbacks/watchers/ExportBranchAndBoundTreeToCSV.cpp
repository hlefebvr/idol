//
// Created by henri on 27.10.23.
//
#include "idol/optimizers/callbacks/watchers/ExportBranchAndBoundTreeToCSV.h"
#include <fstream>

idol::Utils::ExportBranchAndBoundTreeToCSV::ExportBranchAndBoundTreeToCSV(std::string t_filename) : m_filename(std::move(t_filename)) {

}

idol::BranchAndBoundCallbackFactory<idol::DefaultNodeInfo> *idol::Utils::ExportBranchAndBoundTreeToCSV::clone() const {
    return new ExportBranchAndBoundTreeToCSV(*this);
}

idol::BranchAndBoundCallback<idol::DefaultNodeInfo> *idol::Utils::ExportBranchAndBoundTreeToCSV::operator()() {
    return new Strategy(m_filename);
}

idol::Utils::ExportBranchAndBoundTreeToCSV::Strategy::Strategy(std::string t_filename) : m_filename(std::move(t_filename)) {

    std::ofstream file(m_filename);

    if (!file.is_open()) {
        throw Exception("Could not open destination file.");
    }

    file.close();

}

void idol::Utils::ExportBranchAndBoundTreeToCSV::Strategy::operator()(idol::CallbackEvent t_event) {

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
    }

    if (status == Optimal || status == Feasible) {
        sum_of_infeasibilities = node.info().sum_of_infeasibilities();
    }

    file << time().count() << ","
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
