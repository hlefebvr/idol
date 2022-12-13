//
// Created by henri on 13/12/22.
//
#include "algorithms/generation/ColGenSP.h"
#include "algorithms/generation/Generation.h"
#include "modeling/variables/Attributes_Var.h"
#include "modeling/models/Attributes_Model.h"
#include "modeling/expressions/operations/operators_Var.h"

TempVar create_column_template(Algorithm& t_rmp, const Var& t_var) {

    if (t_rmp.get(Attr::Var::Status, t_var) == 0) {
        throw Exception("Trying to add a subproblem associated to a variable which is not in the master problem.");
    }

    return {
        t_rmp.get(Attr::Var::Lb, t_var),
        t_rmp.get(Attr::Var::Ub, t_var),
        t_rmp.get(Attr::Var::Type, t_var),
        Column(t_rmp.get(Attr::Var::Column, t_var))
    };
}

ColGenSP::ColGenSP(Generation<ColGenSP> &t_parent, const Var &t_var)
    : SP(t_parent),
      m_column_template(create_column_template(t_parent.rmp_solution_strategy(), t_var)) {

    t_parent.rmp_solution_strategy().remove(t_var);

}

void ColGenSP::initialize() {

    if (!has_exact_solution_strategy()) {
        throw Exception("No exact solution strategy has been given.");
    }

}

void ColGenSP::update() {

    auto& master = parent().rmp_solution_strategy();
    auto& pricing = exact_solution_strategy();
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

    pricing.set(Attr::Obj::Expr, std::move(objective));

}

void ColGenSP::enrich_master_problem() {

    auto& master = parent().rmp_solution_strategy();
    auto last_pricing_solution = exact_solution_strategy().primal_solution();
    auto alpha = master.add_var(
                TempVar(
                    m_column_template.lb(),
                    m_column_template.ub(),
                    m_column_template.type(),
                    m_column_template.column().fix(last_pricing_solution)
                )
            );

}
