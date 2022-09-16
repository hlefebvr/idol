//
// Created by henri on 14/09/22.
//
#include <cmath>
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/AbstractNode.h"
#include "solvers/solutions/Solution.h"
#include "algorithms/logs/Log.h"

MostInfeasible::MostInfeasible(std::vector<Var> t_branching_candidates)
    : m_branching_candidates(std::move(t_branching_candidates)) {

}

double MostInfeasible::fractional_part(double t_x) {
    return std::abs(std::round(t_x) - t_x);
}

bool MostInfeasible::is_integer(double t_x) {
    return fractional_part(t_x) <= ToleranceForIntegrality;
}

bool MostInfeasible::is_valid(const AbstractNode &t_node) const {
    const auto& primal = t_node.primal_solution();

    for (const auto& var : m_branching_candidates) {
        if (double value = primal.get(var) ; !is_integer(value)) {
            EASY_LOG(Trace, "branch-and-bound", "Node " << t_node.id() << " solution not valid (value = " << value << ")." );
            return false;
        }
    }

    return true;
}

double MostInfeasible::score(const Var &t_var, const AbstractNode& t_node) {
    const double frac_value = fractional_part(t_node.primal_solution().get(t_var));
    if (frac_value <= ToleranceForIntegrality) {
        return -Inf;
    }
    return std::max(frac_value, 1. - frac_value);
}

std::list<AbstractNode *> MostInfeasible::create_child_nodes(unsigned int t_id, const AbstractNode& t_node) {

    double max_infeas = 0.;
    const Var* selected_var = nullptr;

    for (const auto& var : m_branching_candidates) {
        if (double infeas = score(var, t_node) ; infeas > max_infeas) {
            max_infeas = infeas;
            selected_var = &var;
        }
    }

    if (!selected_var || max_infeas <= ToleranceForIntegrality) {
        throw std::runtime_error("Maximum infeasibility is less than ToleranceForIntegrality");
    }

    const double value = t_node.primal_solution().get(*selected_var);

    auto* n1 = t_node.create_child(t_id);
    n1->set_local_lower_bound(*selected_var, std::ceil(value));

    auto* n2 = t_node.create_child(t_id+1);
    n2->set_local_upper_bound(*selected_var, std::floor(value));

    return { n1, n2 };
}
