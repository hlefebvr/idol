//
// Created by henri on 05/01/23.
//
#include "algorithms/benders/BendersSP.h"
#include "algorithms/benders/Benders.h"
#include "modeling/expressions/operations/operators.h"

TempCtr extract_cut_template(Algorithm& t_master, unsigned int t_index, const Ctr& t_cut) {

    TempCtr result {
        Row(t_master.get(Attr::Ctr::Row, t_cut)),
        t_master.get(Attr::Ctr::Type, t_cut)
    };

    t_master.remove(t_cut);

    return result;
}

BendersSP::BendersSP(Benders &t_parent, unsigned int t_index, Model& t_model, const Ctr& t_cut, const Var& t_epigraph)
    : m_parent(t_parent),
      m_index(t_index),
      m_model(t_model),
      m_cut_template(extract_cut_template(t_parent.master_solution_strategy(), t_index, t_cut)),
      m_epigraph_variable(t_epigraph) {

}

Model &BendersSP::model() {
    return m_model;
}

void BendersSP::initialize() {

    if (!m_exact_solution_strategy) {
        throw Exception("No solution strategy at hand for subproblem " + std::to_string(m_index) + ".");
    }

}

void BendersSP::update(const Solution::Primal &t_master_solution) {

    double sign = m_cut_template.type() == LessOrEqual ? 1. : -1.;

    Expr objective = sign * m_cut_template.row().rhs().numerical();

    for (const auto& [param, coeff] : m_cut_template.row().rhs()) {
        objective += sign * coeff * param.as<Var>();
    }

    for (const auto &[var, constant]: m_cut_template.row().linear()) {
        objective += -sign * constant.numerical() * t_master_solution.get(var);
        for (const auto &[param, coeff]: constant) {
            objective += -sign * t_master_solution.get(var) * coeff * param.as<Var>();
        }
    }

    m_exact_solution_strategy->set(Attr::Obj::Expr, objective);
}

void BendersSP::solve() {

    const double remaining_time = m_parent.remaining_time();
    m_exact_solution_strategy->set(Param::Algorithm::TimeLimit, remaining_time);
    m_exact_solution_strategy->solve();

}

SolutionStatus BendersSP::status() const {
    return m_exact_solution_strategy->status();
}

double BendersSP::objective_value() const {
    return m_exact_solution_strategy->get(Attr::Solution::ObjVal);
}

void BendersSP::enrich_master_problem() {

    const bool use_ray = m_exact_solution_strategy->status() == Unbounded;

    auto& master = m_parent.master_solution_strategy();
    auto generator = use_ray ? m_exact_solution_strategy->unbounded_ray() : m_exact_solution_strategy->primal_solution();

    auto temp_cut = create_cut_from_generator(generator);
    if (use_ray) {
        temp_cut.row().linear().remove(m_epigraph_variable);
    }
    idol_Log(Trace, Benders, "Adding new cut: " << temp_cut);
    auto cut = master.add_ctr( std::move(temp_cut) );

    m_pool.add(cut, std::move(generator));
    m_present_generators.emplace_back(cut, m_pool.last_inserted());
}

TempCtr BendersSP::create_cut_from_generator(const Solution::Primal &t_generator) const {
    return {
      m_cut_template.row().fix(t_generator),
      m_cut_template.type()
    };
}
