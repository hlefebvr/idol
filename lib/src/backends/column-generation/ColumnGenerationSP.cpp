//
// Created by henri on 08/02/23.
//
#include "backends/column-generation/ColumnGenerationSP.h"
#include "backends/column-generation/ColumnGeneration.h"
#include "modeling/expressions/operations/operators.h"

impl::ColumnGenerationSP::ColumnGenerationSP(ColumnGeneration &t_parent, unsigned int t_index)
    : m_parent(t_parent),
      m_index(t_index),
      m_model(t_parent.parent().block(t_index).model().clone()) {

}

void impl::ColumnGenerationSP::update_objective(bool t_farkas_pricing, const Solution::Dual &t_duals) {

    ::Expr<Var, Var> objective;

    const auto& column = m_parent.parent().block(m_index).generation_pattern();

    for (const auto &[ctr, constant] : column.linear()) {
        objective += constant.numerical() * -t_duals.get(ctr);
        for (const auto &[param, coeff]: constant) {
            objective += -t_duals.get(ctr) * coeff * param.as<Var>();
        }
    }

    if (!t_farkas_pricing) {
        for (const auto &[param, coeff] : column.obj()) {
            objective += coeff * param.as<Var>();
        }
    }

    m_model->set(::Attr::Obj::Expr, std::move(objective));

}

void impl::ColumnGenerationSP::solve() {

    const double remaining_time = m_parent.parent().remaining_time();
    m_model->set(::Param::Algorithm::TimeLimit, remaining_time);
    m_model->optimize();

}

double impl::ColumnGenerationSP::compute_reduced_cost(const Solution::Dual &t_duals) const {

    double result = 0.;

    const auto& column = m_parent.parent().block(m_index).generation_pattern();
    const auto& primals = save(*m_model, ::Attr::Solution::Primal);

    for (const auto &[ctr, constant] : column.linear()) {
        result += constant.numerical() * -t_duals.get(ctr);
        for (const auto &[param, coeff]: constant) {
            result += -t_duals.get(ctr) * coeff * primals.get(param.as<Var>());
        }
    }

    for (const auto &[param, coeff] : column.obj()) {
        result += coeff * primals.get(param.as<Var>());
    }

    return result;
}

void impl::ColumnGenerationSP::enrich_master_problem() {

    auto& master = m_parent.master();
    auto generator = save(*m_model, ::Attr::Solution::Primal);

    auto alpha = create_column_from_generator(generator);

    master.add(alpha);
    m_pool.add(alpha, std::move(generator));
    m_present_generators.emplace_back(alpha, m_pool.last_inserted());
}

Var impl::ColumnGenerationSP::create_column_from_generator(const Solution::Primal &t_primals) const {

    return {
        m_model->env(),
        0.,
        Inf,
        Continuous,
        m_parent.parent().block(m_index).generation_pattern().fix(t_primals)
    };

}

void impl::ColumnGenerationSP::clean_up() {

    const unsigned int threshold = m_parent.get(::Param::ColumnGeneration::CleanUpThreshold);

    if (m_pool.size() < threshold) {
        return;
    }

    auto& master = m_parent.master();
    const double ratio = m_parent.get(::Param::ColumnGeneration::CleanUpRatio);
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
