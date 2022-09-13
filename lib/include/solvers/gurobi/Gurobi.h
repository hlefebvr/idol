//
// Created by henri on 09/09/22.
//

#ifndef OPTIMIZE_GUROBI_H
#define OPTIMIZE_GUROBI_H
#ifdef USE_GUROBI

#include "solvers/BaseSolver.h"
#include <gurobi_c++.h>

class Gurobi final : public BaseSolver<GRBVar, GRBConstr> {
    static GRBEnv m_env;
    GRBModel m_model;
public:
    explicit Gurobi(Model& t_model);

    void write(const std::string &t_filename) override;

    void solve() override;

    GRBEnv& get_env() { return m_env; }

    [[nodiscard]] const GRBEnv& get_env() const { return m_env; }

    GRBModel& get_model() { return m_model; }

    [[nodiscard]] const GRBModel& get_model() const { return m_model; }
protected:

    static char gurobi_type(VarType t_type);

    static char gurobi_type(CtrType t_type);

    GRBVar create_variable(const Var &t_var) override;

    GRBConstr create_constraint(const Ctr &t_ctr) override;

    void fill_column(const Var &t_var) override;

    void fill_row(const Ctr &t_ctr) override;

    void remove_variable(const Var &t_var) override;

    void remove_constraint(const Ctr &t_ctr) override;

    void set_objective_coefficient(const Var &t_var, const Coefficient &t_coeff) override;

    void set_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) override;

    void set_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) override;

    void set_lb(const Var &t_var, double t_lb) override;

    void set_ub(const Var &t_var, double t_ub) override;

    void set_type(const Var &t_var, VarType t_type) override;

    void set_type(const Ctr &t_ctr, CtrType t_type) override;

    [[nodiscard]] SolutionStatus get_primal_status() const override;

    [[nodiscard]] double get_primal_objective_value() const override;

    [[nodiscard]] double get_primal_value(const Var &t_var) const override;

    [[nodiscard]] double get_extreme_ray_value(const Var &t_var) const override;

    [[nodiscard]] double get_dual_value(const Ctr &t_var) const override;

    [[nodiscard]] double get_dual_farkas_objective_value() const override;

    [[nodiscard]] double get_dual_farkas_value(const Ctr &t_ctr) const override;

public:
    [[nodiscard]] bool infeasible_or_unbounded_info() const override;

    void set_infeasible_or_unbounded_info(bool t_value) override;
};

#endif
#endif //OPTIMIZE_GUROBI_H