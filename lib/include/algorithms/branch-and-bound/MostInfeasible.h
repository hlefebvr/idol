//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_MOSTINFEASIBLE_H
#define OPTIMIZE_MOSTINFEASIBLE_H

#include "AbstractBranchingStrategy.h"
#include "modeling/variables/Variable.h"
#include <vector>

class MostInfeasible : public AbstractBranchingStrategy {
    std::vector<Var> m_branching_candidates;

    static double fractional_part(double t_x);
    static bool is_integer(double t_x);
public:
    explicit MostInfeasible(std::vector<Var> t_branching_candidates);

    [[nodiscard]] bool is_valid(const Node &t_node) const override;

    std::list<Node *> create_child_nodes(unsigned int t_id, const Node& t_node) override;
};

#endif //OPTIMIZE_MOSTINFEASIBLE_H
