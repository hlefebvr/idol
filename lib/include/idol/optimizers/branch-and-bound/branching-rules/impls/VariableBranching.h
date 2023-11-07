//
// Created by henri on 16.10.23.
//

#ifndef IDOL_VARIABLEBRANCHING_BRANCHINGRULE_H
#define IDOL_VARIABLEBRANCHING_BRANCHINGRULE_H

#include <list>
#include "BranchingRule.h"
#include "idol/modeling/variables/Var.h"
#include "idol/modeling/solutions/Solution.h"

namespace idol::BranchingRules {
    template<class>
    class VariableBranching;
}

template<class NodeVarInfoT>
class idol::BranchingRules::VariableBranching : public BranchingRule<NodeVarInfoT> {
    std::list<Var> m_branching_candidates;
public:
    using NodeInfoT = NodeVarInfoT;

    virtual bool is_valid(const Node<NodeVarInfoT> &t_node) const {

        const auto& primal_solution = t_node.info().primal_solution();

        for (const auto& var : m_branching_candidates) {
            if (const double value = primal_solution.get(var) ; !is_integer(value, Tolerance::Integer)) {
                return false;
            }
        }

        return true;
    }

    virtual std::list<std::pair<Var, double>> scoring_function(const std::list<Var>& t_variables, const Node<NodeVarInfoT> &t_node) = 0;

    virtual std::list<NodeVarInfoT *> create_child_nodes_for_selected_variable(const Node<NodeVarInfoT> &t_node, const Var& t_var) {

        const auto& primal_solution = t_node.info().primal_solution();
        const double value = primal_solution.get(t_var);
        const double lb = std::ceil(value);
        const double ub = std::floor(value);

        auto* n1 = t_node.info().create_child();
        n1->set_local_lower_bound(t_var, lb);

        auto* n2 = t_node.info().create_child();
        n2->set_local_upper_bound(t_var, ub);

        idol_Log(Trace,
                 "Node " << t_node.id() << " has 2 child nodes with "
                         << t_var << " >= " << lb
                         << " and "
                         << t_var << " <= " << ub
                         << " (current value of " << t_var << " is " << value << ").";
        );

        return { n1, n2 };
    }

    virtual std::list<NodeVarInfoT *> create_child_nodes(const Node<NodeVarInfoT> &t_node) {

        const auto& primal_solution = t_node.info().primal_solution();

        auto invalid_variables = get_invalid_variables(primal_solution);

        Var selected_variable = invalid_variables.front();

        if (invalid_variables.size() > 1) {
            auto scores = scoring_function(invalid_variables, t_node);
            selected_variable = get_argmax_score(scores);
        }

        return create_child_nodes_for_selected_variable(t_node, selected_variable);
    }

    [[nodiscard]] const std::list<Var>& branching_candidates() const { return m_branching_candidates; }

    VariableBranching(const Optimizers::BranchAndBound<NodeVarInfoT>& t_parent, std::list<Var> t_branching_candidates)
        : BranchingRule<NodeVarInfoT>(t_parent),
          m_branching_candidates(std::move(t_branching_candidates)) {

    }

protected:
    std::list<Var> get_invalid_variables(const Solution::Primal& t_primal_solution) {

        std::list<Var> result;

        for (const auto& var : m_branching_candidates) {
            if (const double value = t_primal_solution.get(var) ; !is_integer(value, Tolerance::Integer)) {
                result.emplace_back(var);
            }
        }

        return result;
    }

    Var get_argmax_score(const std::list<std::pair<Var, double>>& t_scores) {

        if (t_scores.empty()) {
            throw Exception("VariableScoringFunction returned an empty list.");
        }

        double max = std::numeric_limits<double>::lowest();
        std::optional<Var> argmax;

        for (const auto& [var, score] : t_scores) {
            if (max < score) {
                max = score;
                argmax = var;
            }
        }

        if (!argmax.has_value()) {
            throw Exception("Could not compute argmax of scores while searching for branching variable.");
        }

        return argmax.value();
    }

};

#endif //IDOL_VARIABLEBRANCHING_BRANCHINGRULE_H
