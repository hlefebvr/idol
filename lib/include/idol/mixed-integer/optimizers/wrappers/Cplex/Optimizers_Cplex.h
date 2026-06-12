//
// Created by Henri on 12/06/2026.
//

#ifndef IDOL_OPTIMIZERS_CPLEX_H
#define IDOL_OPTIMIZERS_CPLEX_H

#include <memory>

#include "idol/general/optimizers/OptimizerWithLazyUpdates.h"
#include "headers/header_cplex.h"

namespace idol::Optimizers {
    class Cplex;
}

class idol::Optimizers::Cplex : public OptimizerWithLazyUpdates<int, int, int, int> {

    class DynamicLib;
    static std::unique_ptr<DynamicLib> m_dynamic_lib;

    CPXENVptr m_env = nullptr;
    CPXLPptr  m_model = nullptr;
    bool m_continuous_relaxation;
    unsigned int m_solution_index = 0;
    bool m_has_quad_obj = false;

    [[nodiscard]] char cplex_var_type(int t_type) const;
    static char cplex_ctr_type(int t_type);
    static int  cplex_obj_sense(int t_sense);
    static double cplex_numeric(double t_value);
    [[nodiscard]] std::pair<SolutionStatus, SolutionReason> cplex_status(int t_status) const;

protected:
    void hook_build() override;
    void hook_optimize() override;
    void hook_write(const std::string &t_name) override;
    int hook_add(const Var& t_var, bool t_add_column) override;
    int hook_add(const Ctr& t_ctr) override;
    int hook_add(const QCtr& t_ctr) override;
    int hook_add(const SOSCtr& t_ctr) override;
    void hook_update(const Var& t_var) override;
    void hook_update(const Ctr& t_ctr) override;
    void hook_update_objective_sense() override;
    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) override;
    void hook_update_objective() override;
    void hook_update_rhs() override;
    void hook_update() override;
    void hook_remove(const Var& t_var) override;
    void hook_remove(const Ctr& t_ctr) override;
    void hook_remove(const QCtr& t_ctr) override;
    void hook_remove(const SOSCtr& t_ctr) override;

    [[nodiscard]] SolutionStatus get_status() const override;
    [[nodiscard]] SolutionReason get_reason() const override;
    [[nodiscard]] double get_best_obj() const override;
    [[nodiscard]] double get_best_bound() const override;
    [[nodiscard]] double get_var_primal(const Var &t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var &t_var) const override;
    [[nodiscard]] double get_var_ray(const Var &t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_relative_gap() const override;
    [[nodiscard]] double get_absolute_gap() const override;
    [[nodiscard]] unsigned int get_n_solutions() const override;
    [[nodiscard]] unsigned int get_solution_index() const override;

    void set_solution_index(unsigned int t_index) override;

public:
    Cplex(const Model& t_model, bool t_continuous_relaxation);
    ~Cplex() override;

    CPXENVptr  env()        { return m_env; }
    CPXCENVptr env()  const { return m_env; }
    CPXLPptr   model()      { return m_model; }
    CPXCLPptr  model() const { return m_model; }

    [[nodiscard]] std::string name() const override { return "Cplex"; }

    void set_max_n_solution_in_pool(unsigned int t_value);

    void set_tol_mip_relative_gap(double t_relative_gap_tolerance) override;
    void set_tol_mip_absolute_gap(double t_absolute_gap_tolerance) override;
    void set_tol_integer(double t_tol_integer) override;
    void set_tol_feasibility(double t_tol_feasibility) override;
    void set_tol_optimality(double t_tol_optimality) override;

    void set_param_logs(bool t_value) override;
    void set_param_presolve(bool t_value) override;
    void set_param_time_limit(double t_time_limit) override;
    void set_param_threads(unsigned int t_thread_limit) override;
    void set_param_best_bound_stop(double t_best_bound_stop) override;
    void set_param_best_obj_stop(double t_best_obj_stop) override;
    void set_param_infeasible_or_unbounded_info(bool t_value) override;

    void set_param(const std::string& t_param, int t_value);
    void set_param(const std::string& t_param, double t_value);

    static bool is_available();
    static std::string get_version();

    static DynamicLib& get_dynamic_lib(bool t_load_library = true);
};

#define CPLEX_SYM_PTR(name) \
typedef decltype(::idol_##name)* idol_##name##_t; \
idol_##name##_t name = nullptr

class idol::Optimizers::Cplex::DynamicLib {
    void* m_handle = nullptr;

    static std::string find_library();
public:
    CPLEX_SYM_PTR(CPXopenCPLEX);
    CPLEX_SYM_PTR(CPXcloseCPLEX);
    CPLEX_SYM_PTR(CPXcreateprob);
    CPLEX_SYM_PTR(CPXfreeprob);
    CPLEX_SYM_PTR(CPXaddcols);
    CPLEX_SYM_PTR(CPXaddrows);
    CPLEX_SYM_PTR(CPXchgbds);
    CPLEX_SYM_PTR(CPXchgctype);
    CPLEX_SYM_PTR(CPXchgobj);
    CPLEX_SYM_PTR(CPXchgobjoffset);
    CPLEX_SYM_PTR(CPXchgobjsen);
    CPLEX_SYM_PTR(CPXchgrhs);
    CPLEX_SYM_PTR(CPXchgsense);
    CPLEX_SYM_PTR(CPXchgcoef);
    CPLEX_SYM_PTR(CPXdelcols);
    CPLEX_SYM_PTR(CPXdelrows);
    CPLEX_SYM_PTR(CPXlpopt);
    CPLEX_SYM_PTR(CPXmipopt);
    CPLEX_SYM_PTR(CPXgetstat);
    CPLEX_SYM_PTR(CPXgetobjsen);
    CPLEX_SYM_PTR(CPXgetobjval);
    CPLEX_SYM_PTR(CPXgetbestobjval);
    CPLEX_SYM_PTR(CPXgetmiprelgap);
    CPLEX_SYM_PTR(CPXgetx);
    CPLEX_SYM_PTR(CPXgetpi);
    CPLEX_SYM_PTR(CPXgetdj);
    CPLEX_SYM_PTR(CPXgetray);
    CPLEX_SYM_PTR(CPXdualfarkas);
    CPLEX_SYM_PTR(CPXgetnumcols);
    CPLEX_SYM_PTR(CPXgetnumrows);
    CPLEX_SYM_PTR(CPXsetintparam);
    CPLEX_SYM_PTR(CPXsetdblparam);
    CPLEX_SYM_PTR(CPXwriteprob);
    CPLEX_SYM_PTR(CPXgeterrorstring);
    CPLEX_SYM_PTR(CPXversionnumber);
    CPLEX_SYM_PTR(CPXgetsolnpoolnumsolns);
    CPLEX_SYM_PTR(CPXgetsolnpoolobjval);
    CPLEX_SYM_PTR(CPXgetsolnpoolx);
    CPLEX_SYM_PTR(CPXpopulate);
    CPLEX_SYM_PTR(CPXaddsos);
    CPLEX_SYM_PTR(CPXdelsos);
    CPLEX_SYM_PTR(CPXgetnumsos);
    CPLEX_SYM_PTR(CPXaddqconstr);
    CPLEX_SYM_PTR(CPXdelqconstrs);
    CPLEX_SYM_PTR(CPXgetnumqconstrs);
    CPLEX_SYM_PTR(CPXcopyquad);
    CPLEX_SYM_PTR(CPXqpopt);

    [[nodiscard]] bool is_available() const { return m_handle != nullptr; }

    DynamicLib();
    ~DynamicLib();
};

#endif //IDOL_OPTIMIZERS_CPLEX_H