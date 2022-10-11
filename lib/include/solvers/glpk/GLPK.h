//
// Created by henri on 11/10/22.
//

#ifndef OPTIMIZE_GLPK_H
#define OPTIMIZE_GLPK_H

#include "solvers/BaseSolver.h"
#include "containers/Optional.h"
#include <stack>

struct glp_prob;

class GLPK final : public BaseSolver<int, int> {
    double m_objective_offset = 0.;
    bool m_infeas_or_unbounded_info = false;
    Optional<SolutionStatus> m_solution_status;
    Optional<Solution::Primal> m_ray;
    Optional<Solution::Dual> m_farkas;
    glp_prob* m_model;
    std::stack<int> m_free_columns;
    std::stack<int> m_free_constraints;
public:
    explicit GLPK(Model& t_model);

    ~GLPK() override;

    void write(const std::string &t_filename) override;

    void solve() override;

    void set_infeasible_or_unbounded_info(bool t_value) override;

    bool infeasible_or_unbounded_info() const override;

    void set_algorithm_for_lp(AlgorithmForLP t_algorithm) override;

    void set_presolve(bool t_value) override;

    bool presolve() const override;

    AlgorithmForLP algorithm_for_lp() const override;

    void compute_iis() override;

protected:
    int create_variable(const Var &t_var) override;

    int create_constraint(const Ctr &t_ctr) override;

    void fill_column(const Var &t_var) override;

    void fill_row(const Ctr &t_ctr) override;

    void remove_variable(const Var &t_var) override;

    void remove_constraint(const Ctr &t_ctr) override;

    void set_objective_coefficient(const Var &t_var, const Constant &t_coeff) override;

    void set_objective_offset(const Constant &t_offset) override;

    void set_rhs(const Ctr &t_ctr, const Constant &t_coeff) override;

    void set_coefficient(const Ctr &t_ctr, const Var &t_var, const Constant &t_coefficient) override;

    void set_lb(const Var &t_var, double t_lb) override;

    void set_ub(const Var &t_var, double t_ub) override;

    void set_type(const Var &t_var, VarType t_type) override;

    void set_type(const Ctr &t_ctr, CtrType t_type) override;

    SolutionStatus get_primal_status() const override;

    SolutionStatus get_dual_status() const override;

    double get_primal_objective_value() const override;

    double get_dual_objective_value() const override;

    double get_primal_value(const Var &t_var) const override;

    double get_unbounded_ray(const Var &t_var) const override;

    double get_unbounded_ray_objective_value() const override;

    double get_dual_value(const Ctr &t_ctr) const override;

    double get_dual_farkas_objective_value() const override;

    double get_dual_farkas_value(const Ctr &t_ctr) const override;

    void compute_dual_farkas();

    void compute_unbounded_ray();
};

#endif //OPTIMIZE_GLPK_H
