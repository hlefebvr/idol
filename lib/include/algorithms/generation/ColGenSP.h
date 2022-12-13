//
// Created by henri on 13/12/22.
//

#ifndef IDOL_COLGENSP_H
#define IDOL_COLGENSP_H

#include "SP.h"
#include "../../modeling/variables/TempVar.h"

class ColGen;

class ColGenSP : public SP<ColGen, Var> {
    TempVar m_column_template;
protected:
    std::pair<Var, Solution::Primal> enrich_master_problem_hook() override;

    void update_local_bound_hook(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var& t_var, double t_value) override;

public:
    ColGenSP(ColGen& t_parent, const Var& t_var);

    void initialize() override;

    void update() override;

    void contribute_to_primal_solution(Solution::Primal &t_primal) const override;
};

#endif //IDOL_COLGENSP_H
