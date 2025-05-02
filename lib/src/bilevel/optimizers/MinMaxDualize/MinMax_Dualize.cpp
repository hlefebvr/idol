//
// Created by henri on 29.11.24.
//
#include <cassert>
#include "idol/bilevel/optimizers/MinMaxDualize/MinMax_Dualize.h"
#include "idol/bilevel/optimizers/MinMaxDualize/Optimizers_MinMax_Dualize.h"
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::Bilevel::MinMax::Dualize::Dualize() = default;

idol::Bilevel::MinMax::Dualize::Dualize(const Bilevel::Description &t_description) : m_description(&t_description) {

}

idol::Optimizer *idol::Bilevel::MinMax::Dualize::operator()(const idol::Model &t_model) const {

    if (!m_description) {
        throw Exception("No bilevel description has been set.");
    }

    if (!m_single_level_optimizer) {
        throw Exception("No deterministic optimizer has been set.");
    }

    if (m_bound_provider && m_use_sos1) {
        throw Exception("Cannot use both bound provider and SOS1 constraints.");
    }

    auto* result = new Optimizers::Bilevel::MinMax::Dualize(t_model,
                                                            *m_description,
                                                            *m_single_level_optimizer,
                                                            m_bound_provider,
                                                            m_use_sos1.value_or(false));

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::Bilevel::MinMax::Dualize::clone() const {
    return new Dualize(*this);
}

idol::Bilevel::MinMax::Dualize &
idol::Bilevel::MinMax::Dualize::with_single_level_optimizer(const idol::OptimizerFactory &t_deterministic_optimizer) {
    if (m_single_level_optimizer) {
        throw Exception("Single-level optimizer has already been set.");
    }
    m_single_level_optimizer.reset(t_deterministic_optimizer.clone());
    return *this;
}

idol::Bilevel::MinMax::Dualize::Dualize(const idol::Bilevel::MinMax::Dualize &t_src)
        : OptimizerFactoryWithDefaultParameters<Dualize>(t_src),
          m_description(t_src.m_description),
          m_single_level_optimizer(t_src.m_single_level_optimizer ? t_src.m_single_level_optimizer->clone() : nullptr),
          m_bound_provider(t_src.m_bound_provider ? t_src.m_bound_provider->clone() : nullptr),
          m_use_sos1(t_src.m_use_sos1) {

}

idol::Model
idol::Bilevel::MinMax::Dualize::make_model(const idol::Model &t_model, const idol::Bilevel::Description &t_description, bool t_use_sos1) {

    if (t_model.get_obj_sense() != Minimize) {
        throw Exception("Only minimization problems are supported.");
    }

    auto& env = t_model.env();

    Reformulators::KKT reformulator(t_model, t_description);

    Model result(env);
    reformulator.add_coupling_variables(result);
    reformulator.add_coupling_constraints(result);
    reformulator.add_dual(result, true);
    result.set_obj_sense(Minimize);
    result.set_obj_expr(-result.get_obj_expr());

    if (!t_use_sos1) {
        return std::move(result);
    }

    for (const auto& [pair, coefficient] : result.get_obj_expr()) {

        if (t_model.has(pair.first) && t_model.has(pair.second)) {
            continue;
        }

        std::optional<Var> original_var, dual_var;

        if (t_model.has(pair.first) && result.get_var_type(pair.first) == Binary) {
            original_var = pair.first;
            dual_var = pair.second;
        }

        if (t_model.has(pair.second) && result.get_var_type(pair.second) == Binary) {
            original_var = pair.second;
            dual_var = pair.first;
        }

        if ((!original_var || !dual_var)) {
            throw Exception("Cannot linearize products involving non-binary variables.");
        }

        result.add_sosctr(true, {*original_var, *dual_var}, {1., 2.}, "__prod_" + original_var->name() + "_" + dual_var->name());

    }

    return std::move(result);
}

void idol::Bilevel::MinMax::Dualize::set_bilevel_description(const idol::Bilevel::Description &t_bilevel_description) {
    m_description = &t_bilevel_description;
}

idol::Model
idol::Bilevel::MinMax::Dualize::make_model(const idol::Model &t_model, const idol::Bilevel::Description &t_description,
                                           idol::Reformulators::KKT::BoundProvider &t_bound_provider) {

    if (t_model.get_obj_sense() != Minimize) {
        throw Exception("Only minimization problems are supported.");
    }

    auto& env = t_model.env();

    Reformulators::KKT reformulator(t_model, t_description);

    Model result(env);
    reformulator.add_coupling_variables(result);
    reformulator.add_coupling_constraints(result);
    reformulator.add_dual(result, true);
    reformulator.add_bounds_on_dual_variables(result, t_bound_provider);
    result.set_obj_sense(Minimize);
    result.set_obj_expr(-result.get_obj_expr());

    const auto& obj = result.get_obj_expr();
    AffExpr linearized_objective = obj.affine();

    for (const auto& [pair, coefficient] : obj) {

        if (t_model.has(pair.first) && t_model.has(pair.second)) {
            continue;
        }

        std::optional<Var> original_var, dual_var;

        if (t_model.has(pair.first) && result.get_var_type(pair.first) == Binary) {
            original_var = pair.first;
            dual_var = pair.second;
        }

        if (t_model.has(pair.second) && result.get_var_type(pair.second) == Binary) {
            original_var = pair.second;
            dual_var = pair.first;
        }

        if (!original_var || !dual_var) {
            throw Exception("Cannot linearize products involving non-binary variables.");
        }

        const auto auxiliary_var = result.add_var(-Inf, Inf, Continuous, 0., "__prod_" + original_var->name() + "_" + dual_var->name());

        double lb = result.get_var_lb(*dual_var);
        double ub = result.get_var_ub(*dual_var);

        linearized_objective += coefficient * auxiliary_var;

        result.add_ctr(auxiliary_var <= *original_var * ub);
        result.add_ctr(auxiliary_var >= *original_var * lb);
        result.add_ctr(auxiliary_var <= *dual_var - lb * (1 - *original_var));
        result.add_ctr(auxiliary_var >= *dual_var - ub * (1 - *original_var));

    }

    result.set_obj_expr(std::move(linearized_objective));

    return std::move(result);
}

idol::Bilevel::MinMax::Dualize &
idol::Bilevel::MinMax::Dualize::with_bound_provider(const idol::Reformulators::KKT::BoundProvider &t_bound_provider) {

    if (m_bound_provider) {
        throw Exception("Bound provider has already been set.");
    }

    m_bound_provider.reset(t_bound_provider.clone());

    return *this;
}

idol::Bilevel::MinMax::Dualize &idol::Bilevel::MinMax::Dualize::with_sos1_constraints(bool t_value) {

    if (m_bound_provider) {
        throw Exception("Cannot use both bound provider and SOS1 constraints.");
    }

    m_use_sos1 = t_value;

    return *this;
}
