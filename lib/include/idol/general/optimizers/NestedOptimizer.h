#ifndef IDOL_NESTEDOPTIMIZER_H
#define IDOL_NESTEDOPTIMIZER_H

#include <algorithm>
#include <utility>

#include "Optimizer.h"

namespace idol {

inline void inherit_reformulation_parameters(const Optimizer& t_parent, Optimizer& t_child) {
    if (t_parent.has_explicit_presolve() && !t_child.has_explicit_presolve()) {
        t_child.set_param_presolve(t_parent.get_param_presolve());
    }
    if (t_parent.has_explicit_best_bound_stop() && !t_child.has_explicit_best_bound_stop()) {
        t_child.set_param_best_bound_stop(t_parent.get_param_best_bound_stop());
    }
    if (t_parent.has_explicit_best_obj_stop() && !t_child.has_explicit_best_obj_stop()) {
        t_child.set_param_best_obj_stop(t_parent.get_param_best_obj_stop());
    }
    if (t_parent.has_explicit_tol_mip_relative_gap() && !t_child.has_explicit_tol_mip_relative_gap()) {
        t_child.set_tol_mip_relative_gap(t_parent.get_tol_mip_relative_gap());
    }
    if (t_parent.has_explicit_tol_mip_absolute_gap() && !t_child.has_explicit_tol_mip_absolute_gap()) {
        t_child.set_tol_mip_absolute_gap(t_parent.get_tol_mip_absolute_gap());
    }
    if (t_parent.has_explicit_tol_integer() && !t_child.has_explicit_tol_integer()) {
        t_child.set_tol_integer(t_parent.get_tol_integer());
    }
    if (t_parent.has_explicit_tol_feasibility() && !t_child.has_explicit_tol_feasibility()) {
        t_child.set_tol_feasibility(t_parent.get_tol_feasibility());
    }
    if (t_parent.has_explicit_tol_optimality() && !t_child.has_explicit_tol_optimality()) {
        t_child.set_tol_optimality(t_parent.get_tol_optimality());
    }
}

template<class Callable>
void with_nested_resource_limits(const Optimizer& t_parent,
                                 Optimizer& t_child,
                                 unsigned int t_thread_limit,
                                 double t_time_limit,
                                 Callable&& t_callable) {
    const double time_limit = t_child.get_param_time_limit();
    const unsigned int thread_limit = t_child.get_param_thread_limit();

    t_child.set_param_time_limit(std::min({time_limit, t_time_limit, t_parent.get_remaining_time()}));
    t_child.set_param_threads(std::min(thread_limit, t_thread_limit));

    try {
        std::forward<Callable>(t_callable)();
    } catch (...) {
        t_child.set_param_time_limit(time_limit);
        t_child.set_param_threads(thread_limit);
        throw;
    }

    t_child.set_param_time_limit(time_limit);
    t_child.set_param_threads(thread_limit);
}

template<class Callable>
void with_nested_resource_limits(const Optimizer& t_parent,
                                 Optimizer& t_child,
                                 unsigned int t_thread_limit,
                                 Callable&& t_callable) {
    with_nested_resource_limits(t_parent, t_child, t_thread_limit, t_child.get_param_time_limit(),
                                std::forward<Callable>(t_callable));
}

template<class Callable>
void with_nested_resource_limits(const Optimizer& t_parent, Optimizer& t_child, Callable&& t_callable) {
    with_nested_resource_limits(t_parent, t_child, t_parent.get_param_thread_limit(),
                                std::forward<Callable>(t_callable));
}

}

#endif // IDOL_NESTEDOPTIMIZER_H
