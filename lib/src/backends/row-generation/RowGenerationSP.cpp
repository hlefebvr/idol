//
// Created by henri on 06/03/23.
//
#include "backends/row-generation/RowGenerationSP.h"
#include "backends/row-generation/RowGeneration.h"
#include "modeling/expressions/operations/operators.h"
#include "backends/parameters/Logs.h"

RowGenerationSP::RowGenerationSP(RowGeneration &t_parent, unsigned int t_index) : impl::RowGenerationSP(t_parent, t_index) {

}

impl::RowGenerationSP::RowGenerationSP(RowGeneration &t_parent, unsigned int t_index)
    : m_parent(&t_parent),
      m_index(t_index),
      m_model(t_parent.parent().block(t_index).model().clone()),
      m_generation_pattern(t_parent.parent().block(t_index).generation_pattern()) {

}

void impl::RowGenerationSP::hook_before_solve() {
    std::cout << "rows not restored from pool" << std::endl;
}

void impl::RowGenerationSP::update(const Solution::Primal& t_master_solution) {

    ::Expr<Var, Var> objective = m_generation_pattern.rhs().numerical();

    for (const auto& [param, coeff] : m_generation_pattern.rhs()) {
        objective += coeff * param.as<Var>();
    }

    for (const auto &[var, constant]: m_generation_pattern.linear()) {
        const double value = t_master_solution.get(var);
        objective += -constant.numerical() * value;
        for (const auto &[param, coeff]: constant) {
            objective += -value * coeff * param.as<Var>();
        }
    }

    m_model->set(::Attr::Obj::Expr, std::move(objective));

}

void impl::RowGenerationSP::solve() {

    const double remaining_time = m_parent->parent().remaining_time();
    m_model->set(::Param::Algorithm::TimeLimit, remaining_time);
    m_model->optimize();

}

void impl::RowGenerationSP::enrich_master_problem() {

    const int status = m_model->get(::Attr::Solution::Status);

    Solution::Primal generator;
    if (status == Unbounded) {
        generator = save(*m_model, ::Attr::Solution::Ray);
    } else {
        generator = save(*m_model, ::Attr::Solution::Primal);
    }

    auto row = create_cut_from_generator(generator);

    if (status == Unbounded) {
        throw Exception("UNBOUNDED SP");
        // row.row().linear().remove(m_epigraph_variable);
    }

    Ctr cut(m_model->env(), GreaterOrEqual, 0);

    auto& master = m_parent->master();

    master.add(cut, std::move(row));
    m_pool.add(cut, std::move(generator));
    m_present_generators.emplace_back(cut, m_pool.last_inserted());

}

TempCtr impl::RowGenerationSP::create_cut_from_generator(const Solution::Primal &t_generator) const {
    return {
            m_generation_pattern.fix(t_generator),
            GreaterOrEqual
    };
}
