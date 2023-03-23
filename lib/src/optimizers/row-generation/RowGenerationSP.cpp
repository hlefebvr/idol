//
// Created by henri on 06/03/23.
//
#include "optimizers/row-generation/RowGenerationSP.h"
#include "optimizers/row-generation/RowGeneration.h"
#include "modeling/expressions/operations/operators.h"
#include "optimizers/parameters/Logs.h"

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
        auto theta = m_parent->parent().block(m_index).aggregator();
        row.row().linear().remove(theta);
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

void impl::RowGenerationSP::apply_lb(const Var &t_var, double t_lb) {

    if (m_parent->parent().get(::Param::RowGeneration::BranchingOnDual)) {

        remove_cut_if([&](const Ctr& t_object, const Solution::Primal& t_generator) {
            return t_generator.get(t_var) < t_lb;
        });

        m_model->set(::Attr::Var::Lb, t_var, t_lb);
        return;
    }

    throw Exception("Branching on primal problem is not yet implemented");
}

void impl::RowGenerationSP::apply_ub(const Var &t_var, double t_ub) {

    if (m_parent->parent().get(::Param::RowGeneration::BranchingOnDual)) {

        remove_cut_if([&](const Ctr& t_object, const Solution::Primal& t_generator) {
            return t_generator.get(t_var) > t_ub;
        });


        m_model->set(::Attr::Var::Ub, t_var, t_ub);
        return;
    }

    throw Exception("Branching on primal problem is not yet implemented");
}

void impl::RowGenerationSP::remove_cut_if(const std::function<bool(const Ctr &, const Solution::Primal &)> &t_indicator_for_removal) {

    auto& master = m_parent->master();

    auto it = m_present_generators.begin();
    const auto end = m_present_generators.end();

    while (it != end) {
        const auto& [cut, generator] = *it;
        if (t_indicator_for_removal(cut, generator)) {
            master.remove(cut);
            it = m_present_generators.erase(it);
        } else {
            ++it;
        }
    }


}
