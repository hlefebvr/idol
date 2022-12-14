//
// Created by henri on 14/12/22.
//
#include "algorithms/generation/DantzigWolfeSP.h"
#include "algorithms/generation/DantzigWolfe.h"

TempVar extract_column_template(Reformulations::DantzigWolfe& t_reformulation, unsigned int t_index) {

    auto& master = t_reformulation.master_problem();
    const auto& alpha = t_reformulation.alpha(t_index);

    TempVar result {
        master.get(Attr::Var::Lb, alpha),
        master.get(Attr::Var::Ub, alpha),
        master.get(Attr::Var::Type, alpha),
        Column(master.get(Attr::Var::Column, alpha))
    };

    master.remove(alpha);

    return result;
}

DantzigWolfeSP::DantzigWolfeSP(DantzigWolfe &t_parent, unsigned int t_index)
    : m_parent(t_parent),
      m_index(t_index),
      m_column_template(extract_column_template(t_parent.reformulation(), t_index)) {

}

void DantzigWolfeSP::initialize() {

    if (!m_exact_solution_strategy) {
        throw Exception("No solution strategy at hand for subproblem " + std::to_string(m_index) + ".");
    }

}

Model &DantzigWolfeSP::model() {
    return m_parent.reformulation().subproblem(m_index);
}

void DantzigWolfeSP::update() {

    auto& master = m_parent.master_solution_strategy();
    const bool use_farkas_pricing = master.status() == Infeasible;
    const Solution::Dual duals = use_farkas_pricing ? master.farkas_certificate() : master.dual_solution();

    Expr objective;

    for (const auto &[ctr, constant] : m_column_template.column().linear()) {
        objective += constant.numerical() * -duals.get(ctr);
        for (const auto &[param, coeff]: constant) {
            objective += -duals.get(ctr) * coeff * param.as<Var>();
        }
    }

    if (!use_farkas_pricing) {
        for (const auto &[param, coeff] : m_column_template.column().obj()) {
            objective += coeff * param.as<Var>();
        }
    }

    m_exact_solution_strategy->set(Attr::Obj::Expr, std::move(objective));

}

void DantzigWolfeSP::solve() {

    m_exact_solution_strategy->solve();

}

bool DantzigWolfeSP::can_enrich_master() {
    return m_exact_solution_strategy->get(Attr::Solution::ObjVal) < -ToleranceForAbsoluteGapPricing;
}

void DantzigWolfeSP::enrich_master_problem() {

    auto& master = m_parent.master_solution_strategy();
    auto generator = m_exact_solution_strategy->primal_solution();

    auto alpha = master.add_var(
            TempVar(
                    m_column_template.lb(),
                    m_column_template.ub(),
                    m_column_template.type(),
                    m_column_template.column().fix(generator)
            )
    );

    m_pool.add(alpha, std::move(generator));
    m_present_generators.emplace_back(alpha, m_pool.last_inserted());
}

SolutionStatus DantzigWolfeSP::status() const {
    return m_exact_solution_strategy->status();
}

void DantzigWolfeSP::contribute_to_primal_solution(Solution::Primal &t_primal) const {

    const auto& rmp = m_parent.master_solution_strategy();
    const auto primals = rmp.primal_solution();

    for (const auto& [alpha, generator] : present_generators()) {
        t_primal += primals.get(alpha) * generator;
    }

}
