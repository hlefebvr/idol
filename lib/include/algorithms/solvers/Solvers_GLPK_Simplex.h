//
// Created by henri on 11/10/22.
//

#ifndef OPTIMIZE_SOLVERS_GLPK_H
#define OPTIMIZE_SOLVERS_GLPK_H

#include "Solver.h"
#include "../attributes/Attributes.h"
#include "../../containers/Optional.h"
#include <stack>

#ifdef IDOL_USE_GLPK

struct glp_prob;

namespace Solvers {
    class GLPK_Simplex;
}

class Solvers::GLPK_Simplex : public Solver<int, int> {

    glp_prob* m_model;
    double m_objective_offset = 0.;

    Optional<SolutionStatus> m_solution_status;
    Optional<Solution::Primal> m_ray;
    Optional<Solution::Dual> m_farkas;

    std::stack<int> m_deleted_variables;
    std::stack<int> m_deleted_constraints;

    Attributes<AttributesSections::Base> m_attributes;
protected:
    AbstractAttributes &attributes() override { return m_attributes; }
    [[nodiscard]] const AbstractAttributes &attributes() const override { return m_attributes; }
    void execute() override;

    int create_variable_impl_with_objective_coefficient(const Var& t_var);

    int create_constraint_impl_with_rhs(const Ctr& t_ctr);

    void set_constraint_lhs(const Ctr& t_ctr);

    void set_variable_components(const Var& t_var);

    void compute_farkas_certificate();

    void compute_unbounded_ray();
public:
    explicit GLPK_Simplex(Model& t_model);

    void update_constraint_rhs(const Ctr &t_ctr, double t_rhs) override;

    void remove_variable(const Var &t_variable) override;

    Solution::Primal primal_solution() const override;

    Solution::Dual dual_solution() const override;

    Solution::Dual farkas_certificate() const override;

    void update_objective(const Row &t_objective) override;

    Var add_column(TempVar t_temporary_variable) override;

    Ctr add_constraint(TempCtr t_temporary_constraint) override;

    void remove_constraint(const Ctr &t_constraint) override;

    void set_lower_bound(const Var &t_var, double t_lb) override;

    void set_upper_bound(const Var &t_var, double t_ub) override;
};

#endif

#endif //OPTIMIZE_SOLVERS_GLPK_H
