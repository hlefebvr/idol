//
// Created by henri on 21/03/23.
//

#ifndef IDOL_VARIABLEBRANCHINGRULE_H
#define IDOL_VARIABLEBRANCHINGRULE_H

#include "BranchingRule.h"
#include "idol/optimizers/Logger.h"
#include "idol/modeling/variables/Var.h"

#include <optional>
#include <list>

namespace idol {
    template<class>
    class BranchAndBound;

    template<class>
    class VariableBranchingRule;
}
template<class NodeVarInfoT>
class idol::VariableBranchingRule : public BranchingRule<NodeVarInfoT> {
    std::optional<std::pair<Var, double>> m_variable_selected_for_branching;
protected:
    void reset_variable_selected_for_branching();

    void set_variable_selected_for_branching(const Var& t_var, double t_score);

    [[nodiscard]] bool has_variable_selected_for_branching() const;

    [[nodiscard]] const std::pair<Var, double>& variable_selected_for_branching() const;

    template<class IteratorT>
    void select_variable_for_branching(IteratorT t_begin,
                                       IteratorT t_end,
                                       const std::function<double(const Var&)>& t_scoring_function
    );

    std::list<NodeVarInfoT*> create_child_nodes_by_bound(
            const Node<NodeVarInfoT>& t_node,
            const Var& t_variable_selected_for_branching,
            double t_value,
            double t_lb,
            double t_ub
    );
public:
    explicit VariableBranchingRule(const Model& t_model) : BranchingRule<NodeVarInfoT>(t_model) {}
};

template<class NodeVarInfoT>
std::list<NodeVarInfoT *> idol::VariableBranchingRule<NodeVarInfoT>::create_child_nodes_by_bound(const Node<NodeVarInfoT> &t_node,
                                                                                     const Var &t_variable_selected_for_branching,
                                                                                     double t_value,
                                                                                     double t_lb,
                                                                                     double t_ub) {
    auto* n1 = t_node.info().create_child();
    n1->set_local_lower_bound(t_variable_selected_for_branching, t_lb);

    auto* n2 = t_node.info().create_child();
    n2->set_local_upper_bound(t_variable_selected_for_branching, t_ub);

    idol_Log(Trace,
             "Node " << t_node.id() << " has 2 child nodes with "
             << t_variable_selected_for_branching << " >= " << t_lb
             << " and "
             << t_variable_selected_for_branching << " <= " << t_ub
             << " (current value of " << t_variable_selected_for_branching << " is " << t_value << ").";
    );

    return { n1, n2 };
}

template<class NodeVarInfoT>
template<class IteratorT>
void idol::VariableBranchingRule<NodeVarInfoT>::select_variable_for_branching(IteratorT t_begin, IteratorT t_end,
                                                                     const std::function<double(
                                                                             const Var &)> &t_scoring_function) {

    IteratorT selected_variable = t_end;

    double maximum_score = -Inf;

    for ( ; t_begin != t_end ; ++t_begin ) {

        // std::cout << *t_begin << " -> " << t_scoring_function(*t_begin) << std::endl;

        if (double variable_score = t_scoring_function(*t_begin) ; variable_score > maximum_score) {

            maximum_score = variable_score;
            selected_variable = t_begin;

        }

    }

    if (selected_variable != t_end) {
        //std::cout << "selected " << *selected_variable << std::endl;
        set_variable_selected_for_branching(*selected_variable, maximum_score);
    }


}

template<class NodeVarInfoT>
const std::pair<idol::Var, double> &idol::VariableBranchingRule<NodeVarInfoT>::variable_selected_for_branching() const {
    return m_variable_selected_for_branching.value();
}

template<class NodeVarInfoT>
bool idol::VariableBranchingRule<NodeVarInfoT>::has_variable_selected_for_branching() const {
    return m_variable_selected_for_branching.has_value();
}

template<class NodeVarInfoT>
void idol::VariableBranchingRule<NodeVarInfoT>::set_variable_selected_for_branching(const Var &t_var, double t_score) {
    m_variable_selected_for_branching = { t_var, t_score };
}

template<class NodeVarInfoT>
void idol::VariableBranchingRule<NodeVarInfoT>::reset_variable_selected_for_branching() {
    m_variable_selected_for_branching = {};
}

#endif //IDOL_VARIABLEBRANCHINGRULE_H
