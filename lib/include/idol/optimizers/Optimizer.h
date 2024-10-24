//
// Created by henri on 31/01/23.
//

#ifndef IDOL_OPTIMIZER_H
#define IDOL_OPTIMIZER_H

#include <string>
#include <optional>

#include "Timer.h"
#include "idol/modeling/numericals.h"
#include "idol/utils/types.h"

namespace idol {
    class Model;
    class Var;
    class Ctr;
    class Constant;

    namespace impl {
        class Optimizer;
    }

    class Optimizer;
}

class idol::impl::Optimizer {
    const ::idol::Model& m_parent;

    bool m_is_terminated = false;

    unsigned int m_param_threads = 1;
    unsigned int m_param_iteration_limit = std::numeric_limits<unsigned int>::max();

    bool m_param_logs = false;
    double m_param_time_limit = std::numeric_limits<double>::max();
    std::optional<double> m_param_best_obj_stop;
    std::optional<double> m_param_best_bound_stop;
    bool m_param_presolve = true;
    bool m_param_infeasible_or_unbounded_info = false;

    double m_tol_mip_relative_gap = Tolerance::MIPRelativeGap;
    double m_tol_mip_absolute_gap = Tolerance::MIPAbsoluteGap;
    double m_tol_integer = Tolerance::Integer;
    double m_tol_feasibility = Tolerance::Feasibility;
    double m_tol_optimality = Tolerance::Optimality;

    Timer m_timer;
protected:
    virtual void build() = 0;

    virtual void add(const Var& t_var) = 0;
    virtual void add(const Ctr& t_ctr) = 0;

    virtual void remove(const Var& t_var) = 0;
    virtual void remove(const Ctr& t_ctr) = 0;

    virtual void update() = 0;

    virtual void write(const std::string& t_name) = 0;

    void optimize();

    virtual void hook_before_optimize() {}

    virtual void hook_optimize() = 0;

    virtual void hook_after_optimize() {}

    virtual void set_solution_index(unsigned int t_index) = 0;

    virtual void update_obj_sense() = 0;

    virtual void update_obj() = 0;

    virtual void update_rhs() = 0;

    virtual void update_obj_constant() = 0;

    virtual void update_mat_coeff(const Ctr& t_ctr, const Var& t_var) = 0;

    virtual void update_ctr_type(const Ctr& t_ctr) = 0;

    virtual void update_ctr_rhs(const Ctr& t_ctr) = 0;

    virtual void update_var_type(const Var& t_var) = 0;

    virtual void update_var_lb(const Var& t_var) = 0;

    virtual void update_var_ub(const Var& t_var) = 0;

    virtual void update_var_obj(const Var& t_var) = 0;
public:
    explicit Optimizer(const ::idol::Model& t_parent);

    virtual ~Optimizer() = default;

    [[nodiscard]] virtual std::string name() const = 0;

    [[nodiscard]] virtual const ::idol::Model& parent() const { return m_parent; }

    [[nodiscard]] virtual SolutionStatus get_status() const = 0;

    [[nodiscard]] virtual SolutionReason get_reason() const = 0;

    [[nodiscard]] virtual double get_best_obj() const = 0;

    [[nodiscard]] virtual double get_best_bound() const = 0;

    [[nodiscard]] virtual double get_var_primal(const Var& t_var) const = 0;

    [[nodiscard]] virtual double get_var_reduced_cost(const Var& t_var) const = 0;

    [[nodiscard]] virtual double get_var_ray(const Var& t_var) const = 0;

    [[nodiscard]] virtual double get_ctr_dual(const Ctr& t_ctr) const = 0;

    [[nodiscard]] virtual double get_ctr_farkas(const Ctr& t_ctr) const = 0;

    [[nodiscard]] virtual double get_relative_gap() const = 0;

    [[nodiscard]] virtual double get_absolute_gap() const = 0;

    [[nodiscard]] virtual unsigned int get_n_solutions() const = 0;

    [[nodiscard]] virtual unsigned int get_solution_index() const = 0;

    [[nodiscard]] bool get_param_logs() const { return m_param_logs; }

    virtual void set_param_logs(bool t_value) { m_param_logs = t_value; }

    [[nodiscard]] double get_param_time_limit() const { return m_param_time_limit; }

    virtual void set_param_time_limit(double t_time_limit) { m_param_time_limit = t_time_limit; }

    [[nodiscard]] unsigned int get_param_thread_limit() const { return m_param_threads; }

    virtual void set_param_threads(unsigned int t_threads) { m_param_threads = t_threads; }

    [[nodiscard]] double get_param_best_obj_stop() const;

    virtual void set_param_best_obj_stop(double t_best_obj_stop) { m_param_best_obj_stop = t_best_obj_stop; }

    [[nodiscard]] double get_param_best_bound_stop() const;

    virtual void set_param_best_bound_stop(double t_best_bound_stop) { m_param_best_bound_stop = t_best_bound_stop; }

    [[nodiscard]] double get_tol_mip_relative_gap() const { return m_tol_mip_relative_gap; }

    virtual void set_tol_mip_relative_gap(double t_tol_mip_relative_gap) { m_tol_mip_relative_gap = t_tol_mip_relative_gap; }

    [[nodiscard]] double get_tol_mip_absolute_gap() const { return m_tol_mip_absolute_gap; }

    virtual void set_tol_mip_absolute_gap(double t_mip_tol_absolute_gap) { m_tol_mip_absolute_gap = t_mip_tol_absolute_gap; }

    [[nodiscard]] double get_tol_feasibility() const { return m_tol_feasibility; }

    virtual void set_tol_feasibility(double t_tol_feasibility) { m_tol_feasibility = t_tol_feasibility; }

    [[nodiscard]] double get_tol_optimality() const { return m_tol_optimality; }

    virtual void set_tol_optimality(double t_tol_optimality) { m_tol_optimality = t_tol_optimality; }

    [[nodiscard]] double get_tol_integer() const { return m_tol_integer; }

    virtual void set_tol_integer(double t_tol_integer) { m_tol_integer = t_tol_integer; }

    [[nodiscard]] unsigned int get_param_iteration_limit() const { return m_param_iteration_limit; }

    void set_param_iteration_limit(unsigned int t_iteration_limit) { m_param_iteration_limit = t_iteration_limit; }

    [[nodiscard]] bool get_param_presolve() const { return m_param_presolve; }

    virtual void set_param_presolve(bool t_value) { m_param_presolve = t_value; }

    [[nodiscard]] bool get_param_infeasible_or_unbounded_info() const { return m_param_infeasible_or_unbounded_info; }

    virtual void set_param_infeasible_or_unbounded_info(bool t_value) { m_param_infeasible_or_unbounded_info = t_value; }

    [[nodiscard]] const Timer& time() const { return m_timer; }

    [[nodiscard]] double get_remaining_time() const;

    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }

    virtual void terminate();

    template<class T> T& as() {
        auto* result = dynamic_cast<T*>(this);
        if (!result) {
            throw Exception("Optimizer could not be cast to desired type.");
        }
        return *result;
    }

    template<class T> const T& as() const {
        auto* result = dynamic_cast<const T*>(this);
        if (!result) {
            throw Exception("Optimizer could not be cast to desired type.");
        }
        return *result;
    }

    template<class T> [[nodiscard]] bool is() const {
        return dynamic_cast<const T*>(this);
    }
};

class idol::Optimizer : public impl::Optimizer {
    friend class Model;
public:
    explicit Optimizer(const ::idol::Model& t_parent);
};

#endif //IDOL_OPTIMIZER_H
