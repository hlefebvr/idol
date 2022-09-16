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
protected:
    virtual double score(const Var& t_var, const AbstractNode& t_node);
public:
    explicit MostInfeasible(std::vector<Var> t_branching_candidates);

    [[nodiscard]] bool is_valid(const AbstractNode &t_node) const override;

    std::list<AbstractNode *> create_child_nodes(unsigned int t_id, const AbstractNode& t_node) override;
};

#endif //OPTIMIZE_MOSTINFEASIBLE_H
