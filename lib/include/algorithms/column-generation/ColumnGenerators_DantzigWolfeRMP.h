//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATORS_DANTZIGWOLFERMP_H
#define OPTIMIZE_COLUMNGENERATORS_DANTZIGWOLFERMP_H

#include "ColumnGenerators_DantzigWolfeSP.h"

namespace ColumnGenerators {
    class DantzigWolfeRMP;
}

class ColumnGenerators::DantzigWolfeRMP : public ColumnGenerators::DantzigWolfeSP {

    Map<Var, Ctr> m_lower_bound_constraints; // SP Var -> LB Ctr
    Map<Var, Ctr> m_upper_bound_constraints; // SP Var -> UB Ctr

    void set_lower_bound_rmp(const Var& t_rmp_variable, double t_lb, ColumnGenerationSP& t_subproblem);

    void set_upper_bound_rmp(const Var& t_rmp_variable, double t_ub, ColumnGenerationSP& t_subproblem);

    void set_bound_rmp(const Var& t_subproblem_variable, double t_bound, Map<Var, Ctr>& t_bound_constraints, const std::function<TempCtr(Expr<Var>&&, double)>& t_ctr_builder, const std::function<double(const Var&)>& t_get_bound, ColumnGenerationSP& t_subproblem);
public:
    DantzigWolfeRMP(Model& t_rmp, const Model& t_subproblem);

    bool set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) override;

    bool set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) override;

    std::optional<Ctr>
    contribute_to_add_constraint(TempCtr &t_temporary_constraint, ColumnGenerationSP &t_subproblem) override;

    bool update_constraint_rhs(const Ctr &t_ctr, double t_rhs, ColumnGenerationSP &t_subproblem) override;

    bool remove_constraint(const Ctr &t_ctr, ColumnGenerationSP &t_subproblem) override;
};

#endif //OPTIMIZE_COLUMNGENERATORS_DANTZIGWOLFERMP_H
