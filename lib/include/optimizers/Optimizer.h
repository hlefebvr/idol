//
// Created by henri on 31/01/23.
//

#ifndef IDOL_OPTIMIZER_H
#define IDOL_OPTIMIZER_H

#include <string>

#include "../modeling/attributes/AttributeManagers_Base.h"
#include "Logger.h"
#include "Timer.h"

class Model;
class Var;
class Ctr;
class Constant;

namespace impl {
    class Optimizer;
}

class impl::Optimizer : protected AttributeManagers::Base {
    const ::Model& m_parent;

    bool m_is_terminated = false;
    LogLevel m_log_level = Mute;
    Color m_log_color = Default;
    double m_time_limit = std::numeric_limits<double>::max();
    unsigned int m_thread_limit = 1;
    unsigned int m_iteration_count_limit = std::numeric_limits<unsigned int>::max();
    double m_best_obj_stop = -Inf;
    double m_best_bound_stop = +Inf;
    double m_relative_gap_tolerance = 1e-4;
    double m_absolute_gap_tolerance = 1e-8;
    bool m_presolve = true;
    bool m_infeasible_or_unbounded_info = false;

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

    static double as_numeric(const Constant& t_constant);
public:
    explicit Optimizer(const ::Model& t_parent);

    [[nodiscard]] virtual std::string name() const = 0;

    [[nodiscard]] virtual const ::Model& parent() const { return m_parent; }

    [[nodiscard]] LogLevel log_level() const { return m_log_level; }

    virtual void set_log_level(LogLevel t_log_level) { m_log_level = t_log_level; }

    [[nodiscard]] Color log_color() const { return m_log_color; }

    virtual void set_log_color(Color t_log_color) { m_log_color = t_log_color; }

    [[nodiscard]] double time_limit() const { return m_time_limit; }

    virtual void set_time_limit(double t_time_limit) { m_time_limit = t_time_limit; }

    [[nodiscard]] unsigned int thread_limit() const { return m_thread_limit; }

    virtual void set_thread_limit(unsigned int t_thread_limit) { m_thread_limit = t_thread_limit; }

    [[nodiscard]] double best_obj_stop() const { return m_best_obj_stop; }

    virtual void set_best_obj_stop(double t_best_obj_stop) { m_best_obj_stop = t_best_obj_stop; }

    [[nodiscard]] double best_bound_stop() const { return m_best_bound_stop; }

    virtual void set_best_bound_stop(double t_best_bound_stop) { m_best_bound_stop = t_best_bound_stop; }

    [[nodiscard]] double relative_gap_tolerance() const { return m_relative_gap_tolerance; }

    void set_relative_gap_tolerance(double t_relative_gap_tolerance) { m_relative_gap_tolerance = t_relative_gap_tolerance; }

    [[nodiscard]] double absolute_gap_tolerance() const { return m_relative_gap_tolerance; }

    void set_absolute_gap_tolerance(double t_relative_gap_tolerance) { m_relative_gap_tolerance = t_relative_gap_tolerance; }

    [[nodiscard]] unsigned int iteration_count_limit() const { return m_iteration_count_limit; }

    void set_iteration_count_limit(unsigned int t_iteration_count_limit) { m_iteration_count_limit = t_iteration_count_limit; }

    [[nodiscard]] bool presolve() const { return m_presolve; }

    virtual void set_presolve(bool t_value) { m_presolve = t_value; }

    [[nodiscard]] bool infeasible_or_unbounded_info() const { return m_infeasible_or_unbounded_info; }

    virtual void set_infeasible_or_unbounded_info(bool t_value) { m_infeasible_or_unbounded_info = t_value; }

    [[nodiscard]] const Timer& time() const { return m_timer; }

    [[nodiscard]] double remaining_time() const;

    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }

    void terminate();

    template<class T> T& as() {
        auto* result = dynamic_cast<T*>(this);
        if (!result) {
            throw Exception("Bad cast.");
        }
        return *result;
    }

    template<class T> const T& as() const {
        auto* result = dynamic_cast<const T*>(this);
        if (!result) {
            throw Exception("Bad cast.");
        }
        return *result;
    }

    template<class T> [[nodiscard]] bool is() const {
        return dynamic_cast<const T*>(this);
    }
};

class Optimizer : public impl::Optimizer {
    friend class Model;
public:
    explicit Optimizer(const ::Model& t_parent);
};

#endif //IDOL_OPTIMIZER_H
