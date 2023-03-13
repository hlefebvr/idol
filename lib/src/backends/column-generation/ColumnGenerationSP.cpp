//
// Created by henri on 08/02/23.
//
#include <cassert>
#include "backends/column-generation/ColumnGenerationSP.h"
#include "backends/column-generation/ColumnGeneration.h"
#include "modeling/expressions/operations/operators.h"

impl::ColumnGenerationSP::ColumnGenerationSP(ColumnGeneration &t_parent, unsigned int t_index)
    : m_parent(&t_parent),
      m_index(t_index),
      m_model(t_parent.parent().block(t_index).model().clone()),
      m_generation_pattern(t_parent.parent().block(t_index).generation_pattern()) {

}

void impl::ColumnGenerationSP::hook_before_solve() {
    restore_column_from_pool();
}

void impl::ColumnGenerationSP::update_objective(bool t_farkas_pricing, const Solution::Dual &t_duals) {

    ::Expr<Var, Var> objective;

    for (const auto &[ctr, constant] : m_generation_pattern.linear()) {
        objective += constant.numerical() * -t_duals.get(ctr);
        for (const auto &[param, coeff]: constant) {
            objective += -t_duals.get(ctr) * coeff * param.as<Var>();
        }
    }

    if (!t_farkas_pricing) {
        for (const auto &[param, coeff] : m_generation_pattern.obj()) {
            objective += coeff * param.as<Var>();
        }
    }

    m_model->set(::Attr::Obj::Expr, std::move(objective));

}

void impl::ColumnGenerationSP::solve() {

    const double remaining_time = m_parent->parent().remaining_time();
    m_model->set(::Param::Algorithm::TimeLimit, remaining_time);
    m_model->optimize();

}

double impl::ColumnGenerationSP::compute_reduced_cost(const Solution::Dual &t_duals) const {

    double result = 0.;

    const auto& primals = save(*m_model, ::Attr::Solution::Primal);

    for (const auto &[ctr, constant] : m_generation_pattern.linear()) {
        result += constant.numerical() * -t_duals.get(ctr);
        for (const auto &[param, coeff]: constant) {
            result += -t_duals.get(ctr) * coeff * primals.get(param.as<Var>());
        }
    }

    for (const auto &[param, coeff] : m_generation_pattern.obj()) {
        result += coeff * primals.get(param.as<Var>());
    }

    return result;
}

void impl::ColumnGenerationSP::enrich_master_problem() {

    const int status = m_model->get(::Attr::Solution::Status);

    Solution::Primal generator;
    if (status == Unbounded) {
        generator = save(*m_model, ::Attr::Solution::Ray);
    } else {
        generator = save(*m_model, ::Attr::Solution::Primal);
    }

    auto column = create_column_from_generator(generator);

    Var alpha(m_model->env(), 0., Inf, Continuous, "_alpha_" + std::to_string(m_index) + "_" + std::to_string(m_pool.size()));

    auto& master = m_parent->master();

    master.add(alpha, std::move(column));
    m_pool.add(alpha, std::move(generator));
    m_present_generators.emplace_back(alpha, m_pool.last_inserted());
}

TempVar impl::ColumnGenerationSP::create_column_from_generator(const Solution::Primal &t_primals) const {

    TempVar result {
        0.,
        Inf,
        Continuous,
        m_generation_pattern.fix(t_primals)
    };

    if (t_primals.status() == Unbounded) {
        auto convexity_constraint = m_parent->parent().block(m_index).aggregator();
        std::cout << convexity_constraint << std::endl;
        result.column().linear().set(convexity_constraint, 0.);
    }

    return result;

}

void impl::ColumnGenerationSP::clean_up() {

    const unsigned int threshold = m_parent->parent().get(::Param::ColumnGeneration::CleanUpThreshold);

    if (m_pool.size() < threshold) {
        return;
    }

    auto& master = m_parent->master();
    const double ratio = m_parent->parent().get(::Param::ColumnGeneration::CleanUpRatio);
    const auto n_to_remove = (unsigned int) (m_pool.size() * (1 - ratio));
    unsigned int n_removed = 0;

    m_present_generators.clear();

    for (auto it = m_pool.values().begin(), end = m_pool.values().end() ; it != end ; ) {

        const bool is_already_in_master = master.has(it->first);
        const bool done_removing = n_removed >= n_to_remove;

        if (done_removing) {

            if (is_already_in_master) {
                m_present_generators.emplace_back(it->first, it->second);
            }

            ++it;
            continue;

        }

        if (is_already_in_master) {

            if (master.get(::Attr::Solution::Primal, it->first) > 0) {

                m_present_generators.emplace_back(it->first, it->second);
                ++it;
                continue;

            }

            master.remove(it->first);

        }

        it = m_pool.erase(it);
        ++n_removed;

    }


}

double impl::ColumnGenerationSP::compute_original_space_primal(const Var &t_var) const {
    double result = 0;
    for (const auto& [alpha, generator] : m_present_generators) {
        const double alpha_val = m_parent->master().get(::Attr::Solution::Primal, alpha);
        if (alpha_val > 0) {
            result += alpha_val * generator.get(t_var);
        }
    }
    return result;
}

void impl::ColumnGenerationSP::apply_lb(const Var &t_var, double t_value) {

    remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
        //return true;
        return t_generator.get(t_var) < t_value;
    });

    if (!m_parent->parent().get(::Param::ColumnGeneration::BranchingOnMaster)) {
        m_model->set(::Attr::Var::Lb, t_var, t_value);
        return;
    }

    apply_bound_on_master(t_var, ::Attr::Var::Lb, t_value);

}

void impl::ColumnGenerationSP::apply_ub(const Var &t_var, double t_value) {

    remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
        //return true;
        return t_generator.get(t_var) > t_value;
    });

    if (!m_parent->parent().get(::Param::ColumnGeneration::BranchingOnMaster)) {
        m_model->set(::Attr::Var::Ub, t_var, t_value);
        return;
    }

    apply_bound_on_master(t_var, ::Attr::Var::Ub, t_value);
}

LinExpr<Var> impl::ColumnGenerationSP::expand(const Var &t_var) const {

    LinExpr<Var> result;

    for (const auto& [alpha, generator] : m_present_generators) {
        result += generator.get(t_var) * alpha;
    }

    return result;
}

void impl::ColumnGenerationSP::apply_bound_on_master(const Var &t_var, const Req<double, Var> &t_attr, double t_value) {

    auto& master = m_parent->master();

    auto& applied_bounds = t_attr == ::Attr::Var::Lb ? m_lower_bound_constraints : m_upper_bound_constraints;
    const auto it = applied_bounds.find(t_var);

    if (it == applied_bounds.end()) { // Create a new constraint

        auto expanded = expand(t_var);
        const int type = t_attr == ::Attr::Var::Lb ? GreaterOrEqual : LessOrEqual;

        Ctr bound_constraint(master.env(), Equal, 0);

        master.add(bound_constraint, TempCtr(::Row(expanded, t_value), type));
        m_generation_pattern.linear().set(bound_constraint, !t_var);

        applied_bounds.emplace(t_var, bound_constraint);

        return;
    }

    const auto& current_rhs = master.get(::Attr::Ctr::Rhs, it->second);

    if (!current_rhs.is_numerical()) {
        throw Exception("Unexpected RHS with non-numerical terms.");
    }

    if (equals(t_value, current_rhs.numerical(), ToleranceForIntegrality)) { // Remove existing constraint for it is not useful anymore

        m_generation_pattern.linear().remove(it->second);
        master.remove(it->second);
        applied_bounds.erase(it);

        return;
    }

    master.set(::Attr::Ctr::Rhs, it->second, t_value);


}

void impl::ColumnGenerationSP::remove_column_if(const std::function<bool(const Var &, const Solution::Primal &)> &t_indicator_for_removal) {

    auto& master = m_parent->master();

    auto it = m_present_generators.begin();
    const auto end = m_present_generators.end();

    while (it != end) {
        const auto& [column_variable, ptr_to_column] = *it;
        if (t_indicator_for_removal(column_variable, ptr_to_column)) {
            master.remove(column_variable);
            it = m_present_generators.erase(it);
        } else {
            ++it;
        }
    }

}

void impl::ColumnGenerationSP::restore_column_from_pool() {

    auto& master = m_parent->master();

    for (auto& [alpha, primal_solution] : m_pool.values()) {

        if (master.has(alpha)) {
            continue;
        }

        bool is_feasible = true;
        for (const auto& var : m_parent->parent().block(m_index).model().vars()) {

            const double lb = m_parent->parent().block(m_index).model().get(::Attr::Var::Lb, var);
            const double ub = m_parent->parent().block(m_index).model().get(::Attr::Var::Ub, var);
            const double value = primal_solution.get(var);

            if (!(lb <= value && value <= ub)) {
                is_feasible = false;
                break;
            }

        }

        if (is_feasible) {
            master.add(alpha, create_column_from_generator(primal_solution));
            m_present_generators.emplace_back(alpha, primal_solution);
        }

    }

}
