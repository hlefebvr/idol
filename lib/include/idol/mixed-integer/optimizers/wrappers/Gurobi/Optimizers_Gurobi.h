//
// Created by henri on 31/01/23.
//

#ifndef IDOL_OPTIMIZERS_GUROBI_H
#define IDOL_OPTIMIZERS_GUROBI_H

#include <memory>

#include "idol/general/optimizers/OptimizerWithLazyUpdates.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"
#include "GurobiCallbackI.h"

namespace idol::Optimizers {
    class Gurobi;
}

namespace idol::impl {
    static int grb_callback(GRBmodel *t_model, void *t_cbdata, int t_where, void *t_usrdata);
}

class idol::Optimizers::Gurobi : public OptimizerWithLazyUpdates<int, int, int, int> {
    friend class ::idol::GurobiCallbackI;
    
    class DynamicLib;
    static std::unique_ptr<DynamicLib> m_dynamic_lib;

    GRBenv* m_env = nullptr;
    GRBmodel* m_model = nullptr;
    bool m_continuous_relaxation;

    std::unique_ptr<GurobiCallbackI> m_gurobi_callback;

    char gurobi_var_type(int t_type);
    static char gurobi_ctr_type(int t_type);
    static char gurobi_obj_sense(int t_sense);
    static double gurobi_numeric(double t_value);
    static VarType idol_var_type(char t_type);
    static CtrType idol_ctr_type(char t_type);
    static ObjectiveSense idol_obj_sense(int t_sense);
    [[nodiscard]] std::pair<SolutionStatus, SolutionReason> gurobi_status(int t_status) const;
protected:
    static DynamicLib& get_dynamic_lib(bool t_load_library = true);
    static GRBenv* get_new_env();

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
    void update_objective_constant();

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
    Gurobi(const Model& t_model, bool t_continuous_relaxation, GRBenv* t_env);
    Gurobi(const Model& t_model, bool t_continuous_relaxation);
    ~Gurobi() override;
    
    GRBenv* env() { return m_env; }
    [[nodiscard]] const GRBenv* env() const { return m_env; }
    GRBmodel* model() { return m_model; }
    [[nodiscard]] const GRBmodel* model() const { return m_model; }

    [[nodiscard]] std::string name() const override { return "Gurobi"; }
    void set_param_time_limit(double t_time_limit) override;
    void set_param_threads(unsigned int t_thread_limit) override;
    void set_param_best_obj_stop(double t_best_obj_stop) override;
    void set_param_best_bound_stop(double t_best_bound_stop) override;
    void set_param_presolve(bool t_value) override;
    void set_param_infeasible_or_unbounded_info(bool t_value) override;
    void add_callback(Callback* t_ptr_to_callback);
    void set_lazy_cut(bool t_value);
    void set_max_n_solution_in_pool(unsigned int t_value);
    void set_param_logs(bool t_value) override;
    void set_param(const std::string& t_param, int t_value);
    void set_param(const std::string& t_param, double t_value);
    void set_tol_mip_relative_gap(double t_relative_gap_tolerance) override;
    void set_tol_mip_absolute_gap(double t_absolute_gap_tolerance) override;
    void set_tol_feasibility(double t_tol_feasibility) override;
    void set_tol_optimality(double t_tol_optimality) override;
    void set_tol_integer(double t_tol_integer) override;

    // static Model read_from_file(Env& t_env, const std::string& t_filename);

    static bool is_available();
    static std::string get_version();
};

#define GUROBI_SYM_PTR(name) \
typedef decltype(::name)* name##_t; \
name##_t name = nullptr

class idol::Optimizers::Gurobi::DynamicLib {
    void* m_handle = nullptr;
    bool m_is_loaded = false;

    static std::string find_library();
public:
    GUROBI_SYM_PTR(GRBversion);
    GUROBI_SYM_PTR(GRBloadenvinternal);
    GUROBI_SYM_PTR(GRBgetintattr);
    GUROBI_SYM_PTR(GRBgeterrormsg);
    GUROBI_SYM_PTR(GRBfreeenv);
    GUROBI_SYM_PTR(GRBgetenv);
    GUROBI_SYM_PTR(GRBnewmodel);
    GUROBI_SYM_PTR(GRBfreemodel);
    GUROBI_SYM_PTR(GRBaddvar);
    GUROBI_SYM_PTR(GRBaddconstr);
    GUROBI_SYM_PTR(GRBaddqconstr);
    GUROBI_SYM_PTR(GRBsetdblattrelement);
    GUROBI_SYM_PTR(GRBsetcharattrelement);
    GUROBI_SYM_PTR(GRBsetobjective);
    GUROBI_SYM_PTR(GRBdelvars);
    GUROBI_SYM_PTR(GRBdelconstrs);
    GUROBI_SYM_PTR(GRBdelsos);
    GUROBI_SYM_PTR(GRBoptimize);
    GUROBI_SYM_PTR(GRBwrite);
    GUROBI_SYM_PTR(GRBsetintattr);
    GUROBI_SYM_PTR(GRBchgcoeffs);
    GUROBI_SYM_PTR(GRBupdatemodel);
    GUROBI_SYM_PTR(GRBsetintparam);
    GUROBI_SYM_PTR(GRBsetdblparam);
    GUROBI_SYM_PTR(GRBsetstrparam);
    GUROBI_SYM_PTR(GRBsetcallbackfunc);
    GUROBI_SYM_PTR(GRBgetintparam);
    GUROBI_SYM_PTR(GRBgetdblparam);
    GUROBI_SYM_PTR(GRBgetstrparam);
    GUROBI_SYM_PTR(GRBgetdblattr);
    GUROBI_SYM_PTR(GRBgetdblattrarray);
    GUROBI_SYM_PTR(GRBgetdblattrelement);
    GUROBI_SYM_PTR(GRBsetdblattr);
    GUROBI_SYM_PTR(GRBdelqconstrs);
    GUROBI_SYM_PTR(GRBaddsos);
    GUROBI_SYM_PTR(GRBcbget);
    GUROBI_SYM_PTR(GRBcbsolution);
    GUROBI_SYM_PTR(GRBcbcut);
    GUROBI_SYM_PTR(GRBcblazy);
    GUROBI_SYM_PTR(GRBterminate);

    [[nodiscard]] bool is_available() const { return m_handle; }
    void load_library();

    DynamicLib();

    ~DynamicLib();
};

#endif //IDOL_OPTIMIZERS_GUROBI_H
