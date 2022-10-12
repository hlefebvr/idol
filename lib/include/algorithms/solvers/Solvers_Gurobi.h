//
// Created by henri on 11/10/22.
//

#ifndef OPTIMIZE_SOLVERS_GUROBI_H
#define OPTIMIZE_SOLVERS_GUROBI_H

#include "Solver.h"
#include "../attributes/Attributes.h"

#ifdef IDOL_USE_GUROBI
#include <gurobi_c++.h>

namespace Solvers {
    class Gurobi;
}

class Solvers::Gurobi : public Solver<GRBVar, GRBConstr> {
    static GRBEnv m_env;
    GRBModel m_model;

    Attributes<AttributesSections::Base> m_attributes;
protected:
    AbstractAttributes &attributes() override { return m_attributes; }
    [[nodiscard]] const AbstractAttributes &attributes() const override { return m_attributes; }
    void execute() override;

    static char gurobi_type(CtrType t_type);

    static char gurobi_type(VarType t_type);

    GRBVar create_variable_impl_with_objective_coefficient(const Var& t_var);

    GRBConstr create_constraint_impl_with_rhs(const Ctr& t_ctr);

    void set_constraint_lhs(const Ctr& t_ctr);

    void set_variable_components(const Var& t_var);

    [[nodiscard]] SolutionStatus solution_status() const;
public:
    explicit Gurobi(Model& t_model);

    void update_constraint_rhs(const Ctr &t_ctr, double t_rhs) override;

    void remove_variable(const Var &t_variable) override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    [[nodiscard]] Solution::Dual farkas_certificate() const override;

    void update_objective(const Row &t_objective) override;

    Var add_column(TempVar t_temporary_variable) override;

    Ctr add_constraint(TempCtr t_temporary_constraint) override;

    void remove_constraint(const Ctr &t_constraint) override;

    void set_lower_bound(const Var &t_var, double t_lb) override;

    void set_upper_bound(const Var &t_var, double t_ub) override;
};

#endif

#endif //OPTIMIZE_SOLVERS_GUROBI_H
