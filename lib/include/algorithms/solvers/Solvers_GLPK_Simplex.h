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
    unsigned int n_solved = 0;

    Optional<SolutionStatus> m_solution_status;
    Optional<Solution::Primal> m_ray;
    Optional<Solution::Dual> m_farkas;

    bool m_rebuild_basis = false;
    std::stack<int> m_deleted_variables;
    std::stack<int> m_deleted_constraints;

    Attributes<AttributesSections::Base> m_attributes;
protected:
    AbstractAttributes &attributes() override { return m_attributes; }
    [[nodiscard]] const AbstractAttributes &attributes() const override { return m_attributes; }
    void execute() override;

    int create(const Var& t_var, bool t_with_collaterals) override;
    int create(const Ctr& t_ctr, bool t_with_collaterals) override;

    void update(const Var& t_var, int& t_impl) override;
    void update(const Ctr& t_ctr, int& t_impl) override;

    void update_obj() override;

    void remove(const Var &t_var, int &t_impl) override;
    void remove(const Ctr &t_ctr, int &t_impl) override;

    void compute_farkas_certificate();

    void compute_unbounded_ray();
public:
    explicit GLPK_Simplex(Model& t_model);

    Solution::Primal primal_solution() const override;

    Solution::Dual dual_solution() const override;

    Solution::Dual farkas_certificate() const override;

    Solution::Primal unbounded_ray() const override;

    using Solver<int, int>::update;

    void update_rhs_coeff(const Ctr &t_ctr, double t_rhs) override;

    void update_var_lb(const Var &t_var, double t_lb) override;

    void update_var_ub(const Var &t_var, double t_ub) override;

    glp_prob* impl() { return m_model; }

    const glp_prob* impl() const { return m_model; }

    using Algorithm::remove;

    void write(const std::string &t_filename) override;
};

#endif

#endif //OPTIMIZE_SOLVERS_GLPK_H
