//
// Created by henri on 16.10.23.
//

#ifndef IDOL_VARIABLEBRANCHING_BRANCHINGRULE_H
#define IDOL_VARIABLEBRANCHING_BRANCHINGRULE_H

#include <list>
#include "BranchingRule.h"
#include "modeling/variables/Var.h"
#include "optimizers/branch-and-bound/scoring-functions/VariableScoringFunction.h"

namespace idol::BranchingRules {
    template<class>
    class VariableBranching;
}
template<class NodeInfoT>
class idol::BranchingRules::VariableBranching : public BranchingRule<NodeInfoT> {
    std::list<Var> m_branching_candidates;
    std::unique_ptr<BranchingRule<NodeInfoT>> m_then;
    std::unique_ptr<VariableScoringFunction> m_scoring_function;
public:
    virtual bool is_valid(const Node<NodeInfoT> &t_node) const {

        const auto& primal_solution = t_node.info().primal_solution();

        for (const auto& var : m_branching_candidates) {
            if (const double value = primal_solution.get(var) ; !is_integer(value, Tolerance::Integer)) {
                return false;
            }
        }

        return !m_then || m_then->is_valid(t_node);
    }

    std::list<Var> get_invalid_variables(const Solution::Primal& t_primal_solution) {

        std::list<Var> result;

        for (const auto& var : m_branching_candidates) {
            if (const double value = t_primal_solution.get(var) ; !is_integer(value, Tolerance::Integer)) {
                result.emplace_back(var);
            }
        }

        return result;
    }

    Var get_argmin_score(const std::list<Var>& t_invalid_variables, const std::list<double>& t_scores) {

        if (t_invalid_variables.size() != t_scores.size()) {
            throw Exception("Invalid variables and scores do not match in size.");
        }

        double min = std::numeric_limits<double>::max();
        std::optional<Var> argmin;

        auto it_vars = t_invalid_variables.begin();
        auto it_scores = t_scores.begin();
        for (auto it_end = t_invalid_variables.end() ; it_vars != it_end ; ++it_vars, ++it_scores ) {
            if (min > *it_scores) {
                min = *it_scores;
                argmin = *it_vars;
            }
        }

        if (!argmin.has_value()) {
            throw Exception("Could not compute argmin of scores while searching for branching variable.");
        }

        return argmin.value();
    }

    virtual std::list<NodeInfoT *> create_child_nodes(const Node<NodeInfoT> &t_node) {

        const auto& primal_solution = t_node.info().primal_solution();

        auto invalid_variables = get_invalid_variables(primal_solution);
        auto scores = (*m_scoring_function)(invalid_variables, primal_solution);
        auto selected_variable = get_argmin_score(invalid_variables, scores);

        const double value = primal_solution.get(selected_variable);
        const double lb = std::ceil(value);
        const double ub = std::floor(value);

        auto* n1 = t_node.info().create_child();
        n1->set_local_lower_bound(selected_variable, lb);

        auto* n2 = t_node.info().create_child();
        n2->set_local_upper_bound(selected_variable, ub);

        idol_Log(Trace,
                 "Node " << t_node.id() << " has 2 child nodes with "
                         << selected_variable << " >= " << lb
                         << " and "
                         << selected_variable << " <= " << ub
                         << " (current value of " << selected_variable << " is " << value << ").";
        );

        return { n1, n2 };
    }

public:
    VariableBranching(const Model& t_model,
                      std::list<Var> t_branching_candidates,
                      BranchingRule<NodeInfoT>* t_then,
                      VariableScoringFunction* t_scoring_function
                      )
        : BranchingRule<NodeInfoT>(t_model),
          m_branching_candidates(std::move(t_branching_candidates)),
          m_then(t_then),
          m_scoring_function(t_scoring_function) {}
};

#endif //IDOL_VARIABLEBRANCHING_BRANCHINGRULE_H
