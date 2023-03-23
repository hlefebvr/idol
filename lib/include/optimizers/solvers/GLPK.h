//
// Created by henri on 14/02/23.
//

#ifndef IDOL_GLPK_H
#define IDOL_GLPK_H

#include <stack>

#ifdef IDOL_USE_GLPK

#include "LazyBackend.h"
#include <glpk.h>

namespace Backends {
    class GLPK;
}

class Backends::GLPK  : public LazyBackend<int, int> {

    glp_prob* m_model;
    glp_smcp m_simplex_parameters;
    glp_iocp m_mip_parameters;
    bool m_solved_as_mip = false;
    bool m_rebuild_basis = false;
    bool m_infeasible_or_unbounded_info = false;

    SolutionStatus m_solution_status = Unknown;
    SolutionReason m_solution_reason = NotSpecified;
    std::optional<Solution::Primal> m_unbounded_ray;
    std::optional<Solution::Dual> m_farkas_certificate;

    std::stack<int> m_deleted_variables;
    std::stack<int> m_deleted_constraints;
protected:
    void hook_build() override;

    void hook_optimize() override;

    void hook_write(const std::string &t_name) override;

    int hook_add(const Var &t_var, bool t_add_column) override;

    int hook_add(const Ctr &t_ctr) override;

    void hook_update_objective_sense() override;

    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) override;

    void hook_update() override;

    void hook_update(const Var &t_var) override;

    void hook_update(const Ctr &t_ctr) override;

    void hook_update_objective() override;

    void hook_update_rhs() override;

    void hook_remove(const Var &t_var) override;

    void hook_remove(const Ctr &t_ctr) override;

    void set_var_attr(int t_index, int t_type, double t_lb, double t_ub, double t_obj);

    void set_ctr_attr(int t_index, int t_type, double t_rhs);

    void save_simplex_solution_status();
    void compute_farkas_certificate();
    void compute_unbounded_ray();
    void save_milp_solution_status();

    using LazyBackend::get;
    using LazyBackend::set;

    void set(const Parameter<bool>& t_param, bool t_value) override;
    void set(const Parameter<double>& t_param, double t_value) override;
    [[nodiscard]] bool get(const Parameter<bool>& t_param) const override;
    [[nodiscard]] double get(const Parameter<double>& t_param) const override;

    [[nodiscard]] int get(const Req<int, void> &t_attr) const override;
    [[nodiscard]] double get(const Req<double, void> &t_attr) const override;
    [[nodiscard]] double get(const Req<double, Var> &t_attr, const Var &t_var) const override;
    [[nodiscard]] double get(const Req<double, Ctr> &t_attr, const Ctr &t_ctr) const override;
public:
    explicit GLPK(const AbstractModel& t_model);
};

#endif

#endif //IDOL_GLPK_H
