//
// Created by henri on 14/09/22.
//
#include <cmath>
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/Node.h"
#include "solvers/solutions/Solution.h"

MostInfeasible::MostInfeasible(std::vector<Var> t_branching_candidates)
    : m_branching_candidates(std::move(t_branching_candidates)) {

}

double MostInfeasible::fractional_part(double t_x) {
    return std::abs(std::round(t_x) - t_x);
}

bool MostInfeasible::is_integer(double t_x) {
    return fractional_part(t_x) <= ToleranceForIntegrality;
}

bool MostInfeasible::is_valid(const Node &t_node) const {
    const auto& primal = t_node.primal_solution();

    for (const auto& var : m_branching_candidates) {
        if (!is_integer(primal.get(var))) {
            return false;
        }
    }

    return true;
}

std::list<Node *> MostInfeasible::create_child_nodes(unsigned int t_id, const Node& t_node) {
    const auto& primal = t_node.primal_solution();

    double max_infeas = 0.;
    const Var* selected_var = nullptr;

    for (const auto& var : m_branching_candidates) {
        const double value = primal.get(var);
        if (is_integer(value)) {
            continue;
        }
        if (double infeas = std::abs(.5 - fractional_part(value)) ; infeas > max_infeas) {
            max_infeas = infeas;
            selected_var = &var;
        }
    }

    if (!selected_var || max_infeas <= ToleranceForIntegrality) {
        throw std::runtime_error("Maximum infeasibility is less than ToleranceForIntegrality");
    }

    const double value = primal.get(*selected_var);

    auto* n1 = t_node.create_child(t_id);
    n1->set_local_lower_bound(*selected_var, std::ceil(value));

    auto* n2 = t_node.create_child(t_id++);
    n2->set_local_upper_bound(*selected_var, std::floor(value));

    return { n1, n2 };
}
