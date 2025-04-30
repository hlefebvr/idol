//
// Created by henri on 29.11.24.
//
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/KKT/Optimizers_KKT.h"
#include "idol/mixed-integer/modeling/models/KKT.h"

idol::Bilevel::KKT::KKT(const Bilevel::Description &t_description) : m_description(&t_description) {

}

idol::Bilevel::KKT::KKT(const idol::Bilevel::Description &t_description,
                        const idol::Reformulators::KKT::BoundProvider &t_bound_provider)
                        : m_description(&t_description),
                          m_bound_provider(t_bound_provider.clone()) {

}

idol::Optimizer *idol::Bilevel::KKT::operator()(const idol::Model &t_model) const {

    if (!m_single_level_optimizer) {
        throw Exception("No deterministic optimizer has been set.");
    }

    if (!m_description) {
        throw Exception("No bilevel description has been set.");
    }

    auto* result = new Optimizers::Bilevel::KKT(t_model,
                                                *m_description,
                                                *m_single_level_optimizer,
                                                m_bound_provider,
                                                m_use_sos1.value_or(false));

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::Bilevel::KKT::clone() const {
    return new KKT(*this);
}

idol::Bilevel::KKT &idol::Bilevel::KKT::with_bound_provider(const Reformulators::KKT::BoundProvider &t_bound_provider) {

    if (m_bound_provider) {
        throw Exception("Big M has already been set.");
    }

    m_bound_provider.reset(t_bound_provider.clone());

    return *this;
}

idol::Bilevel::KKT &
idol::Bilevel::KKT::with_single_level_optimizer(const idol::OptimizerFactory &t_deterministic_optimizer) {
    if (m_single_level_optimizer) {
        throw Exception("KKT optimizer has already been set.");
    }
    m_single_level_optimizer.reset(t_deterministic_optimizer.clone());
    return *this;
}

idol::Bilevel::KKT::KKT(const idol::Bilevel::KKT &t_src)
        : OptimizerFactoryWithDefaultParameters<KKT>(t_src),
          m_description(t_src.m_description),
          m_single_level_optimizer(t_src.m_single_level_optimizer ? t_src.m_single_level_optimizer->clone() : nullptr),
          m_bound_provider(t_src.m_bound_provider ? t_src.m_bound_provider->clone() : nullptr),
          m_use_sos1(t_src.m_use_sos1) {

}

idol::Model
idol::Bilevel::KKT::make_model(const idol::Model &t_model, const idol::Bilevel::Description &t_description, bool t_use_sos1) {

    if (t_model.get_obj_sense() != Minimize) {
        throw Exception("Only minimization problems are supported.");
    }

    auto& env = t_model.env();

    Reformulators::KKT reformulator(t_model, t_description);

    Model result(env);
    reformulator.add_coupling_variables(result);
    reformulator.add_kkt_reformulation(result, t_use_sos1);
    reformulator.add_coupling_constraints(result);
    result.set_obj_expr(t_model.get_obj_expr());

    return result;
}

idol::Model idol::Bilevel::KKT::make_model(const idol::Model &t_model,
                                           const idol::Bilevel::Description &t_description,
                                           Reformulators::KKT::BoundProvider &t_bound_provider) {

    if (t_model.get_obj_sense() != Minimize) {
        throw Exception("Only minimization problems are supported.");
    }

    auto& env = t_model.env();

    Reformulators::KKT reformulator(t_model, t_description);

    Model result(env);
    reformulator.add_coupling_variables(result);
    reformulator.add_kkt_reformulation(result, t_bound_provider);
    reformulator.add_coupling_constraints(result);
    result.set_obj_expr(t_model.get_obj_expr());

    return result;
}

void idol::Bilevel::KKT::set_bilevel_description(const idol::Bilevel::Description &t_bilevel_description) {
    m_description = &t_bilevel_description;
}

idol::Bilevel::KKT &idol::Bilevel::KKT::with_sos1_constraints(bool t_value) {

    if (m_use_sos1) {
        throw Exception("SOS1 has already been set.");
    }

    m_use_sos1 = t_value;

    return *this;
}
