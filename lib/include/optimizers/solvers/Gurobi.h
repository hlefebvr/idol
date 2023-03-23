//
// Created by henri on 31/01/23.
//

#ifndef IDOL_GUROBI_H
#define IDOL_GUROBI_H

#ifdef IDOL_USE_GUROBI
#include <gurobi_c++.h>
#include <memory>
#include <variant>

#include "LazyBackend.h"

class Gurobi : public LazyBackend<GRBVar, std::variant<GRBConstr, GRBQConstr>> {
    static std::unique_ptr<GRBEnv> s_global_env;

    static GRBEnv& get_global_env();

    GRBEnv& m_env;
    GRBModel m_model;

    static char gurobi_var_type(int t_type);
    static char gurobi_ctr_type(int t_type);
    static char gurobi_obj_sense(int t_sense);
    static double gurobi_numeric(double t_value);
    [[nodiscard]] std::pair<char, char> gurobi_status(int t_status) const;
protected:
    void hook_build() override;

    void hook_optimize() override;

    void hook_write(const std::string &t_name) override;

    GRBVar hook_add(const Var& t_var, bool t_add_column) override;

    std::variant<GRBConstr, GRBQConstr> hook_add(const Ctr& t_ctr) override;

    void hook_update(const Var& t_var) override;

    void hook_update(const Ctr& t_ctr) override;

    void hook_update_objective_sense() override;

    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) override;

    void hook_update_objective() override;

    void hook_update_rhs() override;

    void hook_update() override;

    void hook_remove(const Var& t_var) override;

    void hook_remove(const Ctr& t_ctr) override;

    using LazyBackend::get;
    using LazyBackend::set;

    // Solution
    [[nodiscard]] int get(const Req<int, void> &t_attr) const override;
    [[nodiscard]] double get(const Req<double, void>& t_attr) const override;
    [[nodiscard]] double get(const Req<double, Var>& t_attr, const Var& t_var) const override;
    [[nodiscard]] double get(const Req<double, Ctr>& t_attr, const Ctr& t_ctr) const override;

    void set(const Parameter<double>& t_param, double t_value) override;
    [[nodiscard]] double get(const Parameter<double>& t_param) const override;
    void set(const Parameter<bool>& t_param, bool t_value) override;
    [[nodiscard]] bool get(const Parameter<bool>& t_param) const override;
public:
    Gurobi(const AbstractModel& t_model, GRBEnv& t_env);
    explicit Gurobi(const AbstractModel& t_model) : Gurobi(t_model, Gurobi::get_global_env()) {}

    GRBEnv& env() { return m_env; }

    [[nodiscard]] const GRBEnv& env() const { return m_env; }

    GRBModel& model() { return m_model; }

    [[nodiscard]] const GRBModel& model() const { return m_model; }
};

#endif

#endif //IDOL_GUROBI_H
