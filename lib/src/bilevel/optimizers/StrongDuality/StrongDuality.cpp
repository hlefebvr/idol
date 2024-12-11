//
// Created by henri on 29.11.24.
//
#include <cassert>
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/bilevel/optimizers/StrongDuality/Optimizers_StrongDuality.h"
#include "idol/mixed-integer/modeling/models/KKT.h"

idol::Bilevel::StrongDuality::StrongDuality(const Bilevel::Description &t_description) : m_description(t_description) {

}

idol::Optimizer *idol::Bilevel::StrongDuality::operator()(const idol::Model &t_model) const {

    if (!m_single_level_optimizer) {
        throw Exception("No deterministic optimizer has been set.");
    }

    auto* result = new Optimizers::Bilevel::StrongDuality(t_model, m_description, *m_single_level_optimizer);

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::Bilevel::StrongDuality::clone() const {
    return new StrongDuality(*this);
}

idol::Bilevel::StrongDuality &
idol::Bilevel::StrongDuality::with_single_level_optimizer(const idol::OptimizerFactory &t_deterministic_optimizer) {
    if (m_single_level_optimizer) {
        throw Exception("StrongDuality optimizer has already been set.");
    }
    m_single_level_optimizer.reset(t_deterministic_optimizer.clone());
    return *this;
}

idol::Bilevel::StrongDuality::StrongDuality(const idol::Bilevel::StrongDuality &t_src)
        : OptimizerFactoryWithDefaultParameters<StrongDuality>(t_src),
          m_description(t_src.m_description),
          m_single_level_optimizer(t_src.m_single_level_optimizer ? t_src.m_single_level_optimizer->clone() : nullptr) {

}

idol::Model
idol::Bilevel::StrongDuality::make_model(const idol::Model &t_model, const idol::Bilevel::Description &t_description) {

    if (t_model.get_obj_sense() != Minimize) {
        throw Exception("Only minimization problems are supported.");
    }

    auto& env = t_model.env();

    Reformulators::KKT reformulator(t_model, t_description);

    Model result(env);
    reformulator.add_coupling_variables(result);
    reformulator.add_strong_duality_reformulation(result);
    reformulator.add_coupling_constraints(result);
    result.set_obj_expr(result.get_obj_expr());

    return result;
}

