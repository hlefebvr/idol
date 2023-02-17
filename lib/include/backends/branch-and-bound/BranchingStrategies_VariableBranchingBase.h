//
// Created by henri on 01/12/22.
//

#ifndef IDOL_BRANCHINGSTRATEGIES_VARIABLEBRANCHINGBASE_H
#define IDOL_BRANCHINGSTRATEGIES_VARIABLEBRANCHINGBASE_H

#include "BranchingStrategy.h"
#include "modeling/variables/Var.h"
#include "modeling/numericals.h"
#include "errors/Exception.h"
#include "backends/parameters/Logs.h"
#include <optional>

namespace BranchingStrategies::Base {
    class OnVariables;
}

class BranchingStrategies::Base::OnVariables {
public:
    template<class NodeT> class Strategy;
};

template<class NodeT>
class BranchingStrategies::Base::OnVariables::Strategy : public BranchingStrategyWithNodeType<NodeT> {
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

    std::list<NodeT*> create_child_nodes_by_bound(
            const NodeT& t_node,
            const std::function<unsigned int()>& t_id_provider,
            const Var& t_variable_selected_for_branching,
            double t_value,
            double t_lb,
            double t_ub
            );
};

template<class NodeT>
std::list<NodeT *> BranchingStrategies::Base::OnVariables::Strategy<NodeT>::create_child_nodes_by_bound(const NodeT &t_node,
                                                                                                        const std::function<unsigned int()> &t_id_provider,
                                                                                                        const Var &t_variable_selected_for_branching,
                                                                                                        double t_value,
                                                                                                        double t_lb,
                                                                                                        double t_ub) {
    auto* n1 = t_node.create_child( t_id_provider() );
    n1->set_local_lower_bound(t_variable_selected_for_branching, t_lb);

    auto* n2 = t_node.create_child( t_id_provider() );
    n2->set_local_upper_bound(t_variable_selected_for_branching, t_ub);

    idol_Log(Trace,
             BranchAndBound,
             "Node " << t_node.id() << " has 2 child nodes: "
                     << "Node " << n1->id() << " with " << t_variable_selected_for_branching << " >= " << t_lb
                     << " and "
                     << "Node " << n2->id() << " with " << t_variable_selected_for_branching << " <= " << t_ub
                     << " (current value of " << t_variable_selected_for_branching << " is " << t_value << ").";
    );

    return { n1, n2 };
}

template<class NodeT>
template<class IteratorT>
void BranchingStrategies::Base::OnVariables::Strategy<NodeT>::select_variable_for_branching(
            IteratorT t_begin,
            IteratorT t_end,
            const std::function<double(const Var &)> &t_scoring_function
        ) {

    IteratorT selected_variable = t_end;

    double maximum_score = -Inf;

    for ( ; t_begin != t_end ; ++t_begin ) {

        if (double variable_score = t_scoring_function(*t_begin) ; variable_score > maximum_score) {

            maximum_score = variable_score;
            selected_variable = t_begin;

        }

    }

    if (selected_variable != t_end) {
        set_variable_selected_for_branching(*selected_variable, maximum_score);
    }

}

template<class NodeT>
const std::pair<Var, double> &
BranchingStrategies::Base::OnVariables::Strategy<NodeT>::variable_selected_for_branching() const {
    return m_variable_selected_for_branching.value();
}

template<class NodeT>
bool BranchingStrategies::Base::OnVariables::Strategy<NodeT>::has_variable_selected_for_branching() const {
    return m_variable_selected_for_branching.has_value();
}

template<class NodeT>
void BranchingStrategies::Base::OnVariables::Strategy<NodeT>::set_variable_selected_for_branching(const Var &t_var,
                                                                                                  double t_score) {
    m_variable_selected_for_branching = { t_var, t_score };
}

template<class NodeT>
void BranchingStrategies::Base::OnVariables::Strategy<NodeT>::reset_variable_selected_for_branching() {
    m_variable_selected_for_branching = {};
}

#endif //IDOL_BRANCHINGSTRATEGIES_VARIABLEBRANCHINGBASE_H
