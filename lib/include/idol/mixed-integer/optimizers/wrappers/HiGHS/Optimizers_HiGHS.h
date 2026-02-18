//
// Created by henri on 14/02/23.
//

#ifndef IDOL_OPTIMIZERS_HIGHS_H
#define IDOL_OPTIMIZERS_HIGHS_H

#include "idol/general/optimizers/OptimizerWithLazyUpdates.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/headers/header_c_api.h"
#include <stack>

#include "headers/header_HighsInt.h"

namespace idol::Optimizers {
    class HiGHS;
}

class idol::Optimizers::HiGHS  : public OptimizerWithLazyUpdates<int, int, int, int> {

    class DynamicLib;
    static std::unique_ptr<DynamicLib> m_dynamic_lib;

    bool m_continuous_relaxation;

    void* m_model;

    SolutionStatus m_solution_status = Loaded;
    SolutionReason m_solution_reason = NotSpecified;
    double* m_extreme_ray = nullptr;
    double* m_farkas_certificate = nullptr;
    double* m_col_value = nullptr;
    double* m_col_dual = nullptr;
    double* m_row_value = nullptr;
    double* m_row_dual = nullptr;
protected:
    static DynamicLib& get_dynamic_lib();

    void hook_build() override;
    void hook_optimize() override;
    void run_without_presolve();
    void hook_write(const std::string &t_name) override;
    int hook_add(const Var &t_var, bool t_add_column) override;
    int hook_add(const Ctr &t_ctr) override;
    int hook_add(const QCtr &t_ctr) override;
    int hook_add(const SOSCtr &t_ctr) override;
    void hook_update_objective_sense() override;
    void update_objective_constant();
    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) override;
    void hook_update() override;
    void hook_update(const Var &t_var) override;
    void hook_update(const Ctr &t_ctr) override;
    void hook_update_objective() override;
    void hook_update_rhs() override;
    void hook_remove(const Var &t_var) override;
    void hook_remove(const Ctr &t_ctr) override;
    void hook_remove(const QCtr &t_ctr) override;
    void hook_remove(const SOSCtr &t_ctr) override;
    void set_var_attr(int t_index, int t_type, double t_lb, double t_ub, double t_obj);
    void set_var_type(int t_index, int t_type);
    void set_ctr_attr(int t_index, int t_type, double t_rhs);

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

    void analyze_status(HighsInt t_status);
public:
    explicit HiGHS(const Model& t_model, bool t_continuous_relaxation);
    ~HiGHS() override;

    [[nodiscard]] std::string name() const override { return "HiGHS"; }
    void set_param_time_limit(double t_time_limit) override;
    void set_param_best_obj_stop(double t_best_obj_stop) override;
    void set_param_best_bound_stop(double t_best_bound_stop) override;
    void set_param_presolve(bool t_value) override;
    void set_param_logs(bool t_value) override;

};


#define HIGHS_SYM_PTR(name) \
typedef decltype(::name)* name##_t; \
name##_t name = nullptr

class idol::Optimizers::HiGHS::DynamicLib {
    void* m_handle = nullptr;

    static std::string find_library();
public:
    HIGHS_SYM_PTR(Highs_create);
    HIGHS_SYM_PTR(Highs_clearSolver);
    HIGHS_SYM_PTR(Highs_addCol);
    HIGHS_SYM_PTR(Highs_addRow);
    HIGHS_SYM_PTR(Highs_deleteColsByRange);
    HIGHS_SYM_PTR(Highs_deleteRowsByRange);
    HIGHS_SYM_PTR(Highs_changeColBounds);
    HIGHS_SYM_PTR(Highs_changeColCost);
    HIGHS_SYM_PTR(Highs_changeColIntegrality);
    HIGHS_SYM_PTR(Highs_changeRowBounds);
    HIGHS_SYM_PTR(Highs_changeCoeff);
    HIGHS_SYM_PTR(Highs_changeObjectiveSense);
    HIGHS_SYM_PTR(Highs_changeObjectiveOffset);
    HIGHS_SYM_PTR(Highs_getObjectiveValue);
    HIGHS_SYM_PTR(Highs_getNumCol);
    HIGHS_SYM_PTR(Highs_getNumRow);
    HIGHS_SYM_PTR(Highs_getNumRows);
    HIGHS_SYM_PTR(Highs_getInfinity);
    HIGHS_SYM_PTR(Highs_getRunTime);
    HIGHS_SYM_PTR(Highs_getModelStatus);
    HIGHS_SYM_PTR(Highs_run);
    HIGHS_SYM_PTR(Highs_getSolution);
    HIGHS_SYM_PTR(Highs_getPrimalRay);
    HIGHS_SYM_PTR(Highs_getDualRay);
    HIGHS_SYM_PTR(Highs_setBoolOptionValue);
    HIGHS_SYM_PTR(Highs_setDoubleOptionValue);
    HIGHS_SYM_PTR(Highs_setOptionValue);
    HIGHS_SYM_PTR(Highs_getBoolOptionValue);
    HIGHS_SYM_PTR(Highs_writeModel);

    DynamicLib();

    ~DynamicLib();
};


#endif //IDOL_OPTIMIZERS_HIGHS_H
