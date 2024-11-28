//
// Created by henri on 28.11.24.
//
#include "idol/robust/optimizers/affine-decision-rule/AffineDecisionRule.h"
#include "idol/robust/optimizers/affine-decision-rule/Optimizers_AffineDecisionRule.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::Robust::AffineDecisionRule::AffineDecisionRule(const idol::Robust::Description &t_description) : m_description(t_description) {

}

idol::Robust::AffineDecisionRule::AffineDecisionRule(const idol::Robust::AffineDecisionRule &t_src)
        : OptimizerFactoryWithDefaultParameters<AffineDecisionRule>(t_src),
          m_description(t_src.m_description),
          m_deterministic_optimizer(t_src.m_deterministic_optimizer->clone()) {

}

idol::OptimizerFactory *idol::Robust::AffineDecisionRule::clone() const {
    return new AffineDecisionRule(*this);
}

idol::Robust::AffineDecisionRule &idol::Robust::AffineDecisionRule::with_deterministic_optimizer(
        const idol::OptimizerFactory &t_deterministic_optimizer) {
    if (m_deterministic_optimizer) {
        throw Exception("Deterministic optimizer has already been set.");
    }
    m_deterministic_optimizer.reset(t_deterministic_optimizer.clone());
    return *this;
}

idol::Optimizer *idol::Robust::AffineDecisionRule::operator()(const idol::Model &t_model) const {

    if (!m_deterministic_optimizer) {
        throw Exception("No deterministic optimizer has been set.");
    }

    auto* result = new Optimizers::Robust::AffineDecisionRule(t_model, m_description, *m_deterministic_optimizer);

    handle_default_parameters(result);

    return result;
}

idol::Robust::AffineDecisionRule::Result idol::Robust::AffineDecisionRule::make_model(const idol::Model &t_model,
                                                                                      const idol::Robust::Description &t_description) {

    if (t_description.uncertain_mat_coeffs().size() > 0) {
        throw Exception("Cannot handle uncertain matrix coefficients.");
    }

    auto& env = t_model.env();
    const auto& uncertainty_set = t_description.uncertainty_set();

    Result result(t_model.copy(), uncertainty_set);
    result.affine_decision_rules.reserve(t_model.vars().size());

    // Create variables
    for (const auto& var : t_model.vars()) {

        if (t_description.stage(var) == 0) {
            result.affine_decision_rules.emplace_back(var);
            continue;
        }

        QuadExpr<Var> adr = var;
        for (const auto& unc_var : uncertainty_set.vars()) {
            const auto y_tilde = result.model.add_var(-Inf, Inf, Continuous, 0., "adr_" + var.name() + "_" + unc_var.name());
            adr += y_tilde * unc_var;
        }

        result.affine_decision_rules.emplace_back(std::move(adr));
    }

    // Add Uncertain constraints
    for (const auto& ctr : t_model.ctrs()) {

        const auto& row = t_model.get_ctr_row(ctr);

        for (const auto& [var, constant] : row) {

            if (t_description.stage(var) == 0) {
                continue;
            }

            const unsigned int index = t_model.get_var_index(var);
            const auto& adr = result.affine_decision_rules[index];
            for (const auto& [pair, coefficient] : adr) {
                if (uncertainty_set.has(pair.first)) {
                    result.description.set_uncertain_mat_coeff(ctr, pair.second, constant * coefficient * pair.first);
                } else {
                    result.description.set_uncertain_mat_coeff(ctr, pair.first, constant * coefficient * pair.second);
                }
            }

        }

    }

    result.description.set_uncertain_obj(t_description.uncertain_obj());
    result.description.set_uncertain_rhs(t_description.uncertain_rhs());

    return result;

}
