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
        t_primal += primals.get(alpha) * in_original_space(generator);
    }

}

Solution::Primal DantzigWolfeSP::in_original_space(const Solution::Primal &t_primals) const {

    const auto& reformulation = m_parent.reformulation();
    const auto& model = reformulation.subproblem(m_index);
    const auto& original_space_variable = reformulation.original_space_variable(m_index);

    Solution::Primal result;

    for (const auto& [var, val] : t_primals) {
        result.set(model.get<Var>(original_space_variable, var), val);
    }

    return result;
}

void DantzigWolfeSP::apply_original_space_bound_on_master(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {
    throw Exception("Apply bound on master");
}

void DantzigWolfeSP::apply_original_space_bound_on_pricing(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {

    std::cout << "Applying bound for " << t_var << std::endl;

    const auto& reformulation = m_parent.reformulation();
    const auto& reformulated_var = reformulation.original_formulation().get<Var>(reformulation.reformulated_variable(), t_var);

    std::cout << "i.e., " << reformulated_var << std::endl;

    if (t_attr == Attr::Var::Lb) {

        remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
            return t_generator.get(reformulated_var) < t_value;
        });

    } else if (t_attr == Attr::Var::Ub) {

        remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
            return t_generator.get(reformulated_var) > t_value;
        });

    } else {

        throw Exception("Unexpected attribute " + t_attr.name() + ".");

    }

    m_exact_solution_strategy->set(t_attr, reformulated_var, t_value);

}

void DantzigWolfeSP::remove_column_if(const std::function<bool(const Var &, const Solution::Primal &)> &t_indicator_for_removal) {

    auto& rmp = m_parent.master_solution_strategy();

    auto it = m_present_generators.begin();
    const auto end = m_present_generators.end();

    while (it != end) {
        const auto& [column_variable, ptr_to_column] = *it;
        if (t_indicator_for_removal(column_variable, ptr_to_column)) {
            rmp.remove(column_variable);
            it = m_present_generators.erase(it);
        } else {
            ++it;
        }
    }

}
