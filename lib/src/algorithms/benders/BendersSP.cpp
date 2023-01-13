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

void BendersSP::initialize() {

    if (!m_exact_solution_strategy) {
        throw Exception("No solution strategy at hand for subproblem " + std::to_string(m_index) + ".");
    }

}

void BendersSP::update(const Solution::Primal &t_master_solution) {

    Expr objective = m_cut_template.row().rhs().numerical();

    for (const auto& [param, coeff] : m_cut_template.row().rhs()) {
        objective += coeff * param.as<Var>();
    }

    for (const auto &[var, constant]: m_cut_template.row().linear()) {
        const double value = t_master_solution.get(var);
        objective += -constant.numerical() * value;
        for (const auto &[param, coeff]: constant) {
            objective += -value * coeff * param.as<Var>();
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

void BendersSP::set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_bound) {

    if (t_attr == Attr::Var::Lb) {
        set_lower_bound(t_var, t_bound);
        return;
    }

    if (t_attr == Attr::Var::Ub) {
        set_upper_bound(t_var, t_bound);
        return;
    }

    m_exact_solution_strategy->set(t_attr, t_var, t_bound);

}

void BendersSP::set_lower_bound(const Var &t_var, double t_lb) {

    remove_cut_if([&](const Ctr& t_object, const Solution::Primal& t_generator) {
        return t_generator.get(t_var) < t_lb;
    });

    m_exact_solution_strategy->set(Attr::Var::Lb, t_var, t_lb);

}

void BendersSP::set_upper_bound(const Var &t_var, double t_ub) {

    remove_cut_if([&](const Ctr& t_object, const Solution::Primal& t_generator) {
        return t_generator.get(t_var) > t_ub;
    });

    m_exact_solution_strategy->set(Attr::Var::Ub, t_var, t_ub);

}

void
BendersSP::remove_cut_if(const std::function<bool(const Ctr &, const Solution::Primal &)> &t_indicator_for_removal) {

    auto& rmp = m_parent.master_solution_strategy();

    auto it = m_present_generators.begin();
    const auto end = m_present_generators.end();

    while (it != end) {
        const auto& [cut, generator] = *it;
        if (t_indicator_for_removal(cut, generator)) {
            rmp.remove(cut);
            it = m_present_generators.erase(it);
        } else {
            ++it;
        }
    }

}
