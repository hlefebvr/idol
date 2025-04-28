//
// Created by henri on 29.11.24.
//
#include <cassert>
#include "idol/bilevel/optimizers/MinMaxDualize/MinMax_Dualize.h"
#include "idol/bilevel/optimizers/MinMaxDualize/Optimizers_MinMax_Dualize.h"
#include "idol/mixed-integer/modeling/models/KKT.h"

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

    auto* result = new Optimizers::Bilevel::MinMax::Dualize(t_model, *m_description, *m_single_level_optimizer);

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::Bilevel::MinMax::Dualize::clone() const {
    return new Dualize(*this);
}

idol::Bilevel::MinMax::Dualize &
idol::Bilevel::MinMax::Dualize::with_single_level_optimizer(const idol::OptimizerFactory &t_deterministic_optimizer) {
    if (m_single_level_optimizer) {
        throw Exception("StrongDuality optimizer has already been set.");
    }
    m_single_level_optimizer.reset(t_deterministic_optimizer.clone());
    return *this;
}

idol::Bilevel::MinMax::Dualize::Dualize(const idol::Bilevel::MinMax::Dualize &t_src)
        : OptimizerFactoryWithDefaultParameters<Dualize>(t_src),
          m_description(t_src.m_description),
          m_single_level_optimizer(t_src.m_single_level_optimizer ? t_src.m_single_level_optimizer->clone() : nullptr) {

}

idol::Model
idol::Bilevel::MinMax::Dualize::make_model(const idol::Model &t_model, const idol::Bilevel::Description &t_description) {

    if (t_model.get_obj_sense() != Minimize) {
        throw Exception("Only minimization problems are supported.");
    }

    auto& env = t_model.env();

    Reformulators::KKT reformulator(t_model, t_description);

    Model result(env);
    reformulator.add_coupling_variables(result);
    reformulator.add_coupling_constraints(result);
    reformulator.add_dual(result, true);
    result.set_obj_expr(t_model.get_obj_expr());

    return result;
}

void idol::Bilevel::MinMax::Dualize::set_bilevel_description(const idol::Bilevel::Description &t_bilevel_description) {
    m_description = &t_bilevel_description;
}
