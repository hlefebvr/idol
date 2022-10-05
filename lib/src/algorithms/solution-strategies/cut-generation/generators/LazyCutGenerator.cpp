//
// Created by henri on 05/10/22.
//
#include "algorithms/solution-strategies/cut-generation/generators/LazyCutGenerator.h"
#include "modeling/expressions/Row.h"
#include "modeling/solutions/Solution.h"
#include "algorithms/solution-strategies/cut-generation/subproblems/CutGenerationSubproblem.h"
#include "algorithms/logs/Log.h"

LazyCutGenerator::LazyCutGenerator(const Model &t_rmp_model, const Model &t_sp_model)
    : BaseGenerator<Var>(t_rmp_model, t_sp_model) {

}

Row LazyCutGenerator::get_separation_objective(const Solution::Primal &t_primals) {

    double rhs_factor = 1.;

    Row result;

    if (type() == LessOrEqual) {
        result = Row(constant(), 0.);
        rhs_factor = -1;
    } else {
        result = Row(-1. * constant(), 0.);
    }

    for (auto [var, row] : *this) {
        row *= rhs_factor * t_primals.get(var);
        result += row;
    }
    return result;
}

TempCtr LazyCutGenerator::create_cut(const Solution::Primal &t_primals) const {
    Row row;

    double rhs = 0.;
    for (const auto& [var, coeff] : constant()) {
        rhs += coeff.numerical() * t_primals.get(var);
    }
    row.set_rhs(rhs);

    for (const auto& [ctr, expr] : *this) {
        double coeff = expr.rhs().numerical();
        for (const auto& [var, value] : expr.lhs()) {
            coeff += value.numerical() * t_primals.get(var);
        }
        row.lhs().set(ctr, coeff);
    }

    return { std::move(row), type() };
}

bool LazyCutGenerator::set_lower_bound(const Var &t_var, double t_lb, CutGenerationSubproblem &t_subproblem) {

    if (t_var.model_id() != subproblem().id()) { return false; }

    remove_columns_violating_lower_bound(t_var, t_lb, t_subproblem);
    t_subproblem.exact_solution_strategy().set_lower_bound(t_var, t_lb);

    return true;

}

bool LazyCutGenerator::set_upper_bound(const Var &t_var, double t_ub, CutGenerationSubproblem &t_subproblem) {

    if (t_var.model_id() != subproblem().id()) { return false; }

    remove_columns_violating_upper_bound(t_var, t_ub, t_subproblem);
    t_subproblem.exact_solution_strategy().set_upper_bound(t_var, t_ub);

    return true;

}

Solution::Primal LazyCutGenerator::primal_solution(const CutGenerationSubproblem &t_subproblem,
                                                   const Solution::Dual &t_rmp_duals) const {
    Solution::Primal result;

    // unsigned int n_actives = 0;

    double sum_duals = 0.;

    for (const auto& [ctr, primal_solution] : t_subproblem.currently_present_cuts()) {
        const double dual = t_rmp_duals.get(ctr);

        if (!equals(dual, 0., 1e-5)) {
            sum_duals += dual;
            result += dual * primal_solution;
            // ++n_actives;
        }

    }

    result *= 1. / sum_duals;

    return result;
}

void LazyCutGenerator::remove_columns_violating_lower_bound(const Var &t_var, double t_lb, CutGenerationSubproblem &t_subproblem) {

    t_subproblem.remove_cut_if([&](const Ctr& t_cut, const auto& t_cut_primal_solution){
        if (double value = t_cut_primal_solution.get(t_var) ; value < t_lb + ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "cut-generation",
                     "Cut " << t_cut << " was removed by contradiction with required "
                                   << "bound " << t_var << " >= " << t_lb << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}

void LazyCutGenerator::remove_columns_violating_upper_bound(const Var &t_var, double t_ub, CutGenerationSubproblem &t_subproblem) {

    t_subproblem.remove_cut_if([&](const Ctr& t_cut, const auto& t_cut_primal_solution){
        if (double value = t_cut_primal_solution.get(t_var) ; value > t_ub - ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "cut-generation",
                     "Cut " << t_cut << " was removed by contradiction with required "
                               << "bound " << t_var << " <= " << t_ub << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}

