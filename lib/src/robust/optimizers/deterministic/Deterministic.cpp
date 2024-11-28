//
// Created by henri on 28.11.24.
//
#include "idol/robust/optimizers/deterministic/Deterministic.h"
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/robust/optimizers/deterministic/Optimizers_Deterministic.h"

idol::Robust::Deterministic::Deterministic(const Robust::Description &t_description) : m_description(t_description) {

}

idol::Optimizer *idol::Robust::Deterministic::operator()(const idol::Model &t_model) const {

    if (!m_deterministic_optimizer) {
        throw Exception("No deterministic optimizer has been set.");
    }

    auto* result = new Optimizers::Robust::Deterministic(t_model, m_description, *m_deterministic_optimizer);

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::Robust::Deterministic::clone() const {
    return new Deterministic(*this);
}

idol::Robust::Deterministic &
idol::Robust::Deterministic::with_deterministic_optimizer(const idol::OptimizerFactory &t_deterministic_optimizer) {
    if (m_deterministic_optimizer) {
        throw Exception("Deterministic optimizer has already been set.");
    }
    m_deterministic_optimizer.reset(t_deterministic_optimizer.clone());
    return *this;
}

idol::Robust::Deterministic::Deterministic(const idol::Robust::Deterministic &t_src)
    : OptimizerFactoryWithDefaultParameters<Deterministic>(t_src),
      m_description(t_src.m_description),
      m_deterministic_optimizer(t_src.m_deterministic_optimizer->clone()) {

}

idol::Model
idol::Robust::Deterministic::make_model(const idol::Model &t_model, const idol::Robust::Description &t_description) {

    if (t_model.get_obj_sense() != Minimize) {
        throw Exception("Only minimization problems are supported.");
    }

    if (t_model.qctrs().size() > 0) {
        throw Exception("Quadratic constraints are not supported.");
    }

    auto& env = t_model.env();
    const auto& uncertainty_set = t_description.uncertainty_set();
    Model result(env);

    // Add variables
    for (const auto& var : t_model.vars()) {
        const double lb = t_model.get_var_lb(var);
        const double ub = t_model.get_var_ub(var);
        const auto type = t_model.get_var_type(var);
        result.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));
    }

    // Add constraints
    for (const auto& ctr : t_model.ctrs()) {
        const auto& row = t_model.get_ctr_row(ctr);
        const auto rhs = t_model.get_ctr_rhs(ctr);
        const auto type = t_model.get_ctr_type(ctr);

        const auto& uncertain_mat_coeffs = t_description.uncertain_mat_coeffs(ctr);
        const auto& uncertain_rhs = t_description.uncertain_rhs(ctr);

        const bool has_lhs_uncertainty = !uncertain_mat_coeffs.is_zero(Tolerance::Sparsity);
        const bool has_rhs_uncertainty = !uncertain_rhs.is_zero(Tolerance::Sparsity);

        if (!has_lhs_uncertainty && !has_rhs_uncertainty) {
            result.add(ctr, TempCtr(LinExpr(row), type, rhs));
            continue;
        }

        if (type == Equal) {
            throw Exception("Cannot make equality constraints deterministic.");
        }

        QuadExpr<Var> objective = row;
        for (const auto& [var, uncertain_coeff] : row) {
            const auto& uncertain_mat_coeff = uncertain_mat_coeffs.get(var);
            objective +=  uncertain_mat_coeff * var;
        }
        objective -= rhs + uncertain_rhs;

        if (type == LessOrEqual) {
            objective *= -1;
        }

        Reformulators::KKT kkt(uncertainty_set,
                               objective,
                               [&](const Var& t_var) { return uncertainty_set.has(t_var); },
                               [&](const Ctr& t_ctr) { return uncertainty_set.has(t_ctr); },
                               [&](const QCtr& t_qvar) { return uncertainty_set.has(t_qvar); }
        );
        kkt.set_prefix("robust_" + ctr.name() + "_");
        kkt.add_dual_variables(result);
        kkt.add_dual_constraints(result);

        auto dual_objective = kkt.get_dual_obj_expr();
        if (type == LessOrEqual) {
            dual_objective *= -1;
        }

        assert(!dual_objective.has_quadratic());

        result.add(ctr, TempCtr(
                std::move(dual_objective.affine().linear()),
                type,
                -dual_objective.affine().constant())
        );

    }

    // Add objective
    const auto& obj = t_model.get_obj_expr();
    const auto& uncertain_obj = t_description.uncertain_obj();
    if (uncertain_obj.is_zero(Tolerance::Sparsity)) {
        result.set_obj_expr(obj);
    } else {

        QuadExpr<Var> objective = obj;
        for (const auto& [var, coeff] : uncertain_obj) {
            objective += coeff * var;
        }

        objective *= -1; // Minimization problem

        Reformulators::KKT kkt(uncertainty_set,
                               objective,
                               [&](const Var& t_var) { return uncertainty_set.has(t_var); },
                               [&](const Ctr& t_ctr) { return uncertainty_set.has(t_ctr); },
                               [&](const QCtr& t_qvar) { return uncertainty_set.has(t_qvar); }
        );
        kkt.set_prefix("robust_obj_");
        kkt.add_dual_variables(result);
        kkt.add_dual_constraints(result);

        auto dual_objective = kkt.get_dual_obj_expr();
        dual_objective *= -1;

        result.set_obj_expr(dual_objective);

    }

    return result;
}

