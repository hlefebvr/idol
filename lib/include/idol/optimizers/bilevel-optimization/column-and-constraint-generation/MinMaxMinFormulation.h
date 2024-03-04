//
// Created by henri on 04.03.24.
//

#ifndef IDOL_MINMAXMINFORMULATION_H
#define IDOL_MINMAXMINFORMULATION_H

#include "idol/modeling/models/Model.h"
#include <list>

namespace idol::Optimizers::Bilevel {
    class ColumnAndConstraintGeneration;
}

namespace idol::Bilevel::impl {
    class MinMaxMinFormulation;
}

class idol::Bilevel::impl::MinMaxMinFormulation {
    const idol::Optimizers::Bilevel::ColumnAndConstraintGeneration& m_parent;
    const double m_penalty_parameter;
    Model m_uncertainty_set;
    Model m_second_stage_dual;
    Model m_two_stage_robust_formulation;

    void identify_complicating_variables();
    std::list<Var> identify_coupling_variables();
    void identify_easy_constraints();
    bool all_variables_are_in_the_uncertainty_set(const idol::Row& t_row) const;
    void add_ctr_to_second_stage_dual(const idol::Ctr& t_ctr, const idol::Row& t_row);
    void add_penalty_function_to_second_stage_dual(const std::list<idol::Var>& t_coupling_variables);
    void fill_two_stage_robust_formulation();
    Expr<Var, Var> to_two_stage_robust_formulation_space(const Constant& t_src) const;
public:
    MinMaxMinFormulation(const idol::Optimizers::Bilevel::ColumnAndConstraintGeneration& t_parent, double t_penalty_parameter);

    const Model& uncertainty_set() const { return m_uncertainty_set; }

    const Model& second_stage_dual() const { return m_second_stage_dual; }

    const Model& two_stage_robust_formulation() const { return m_two_stage_robust_formulation; }
};

#endif //IDOL_MINMAXMINFORMULATION_H
