//
// Created by henri on 29.11.24.
//
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/KKT/Optimizers_KKT.h"
#include "idol/mixed-integer/modeling/models/KKT.h"

idol::Bilevel::KKT::KKT(const Bilevel::Description &t_description) : m_description(t_description) {

}

idol::Optimizer *idol::Bilevel::KKT::operator()(const idol::Model &t_model) const {

    if (!m_single_level_optimizer) {
        throw Exception("No deterministic optimizer has been set.");
    }

    auto* result = new Optimizers::Bilevel::KKT(t_model,
                                                m_description,
                                                *m_single_level_optimizer,
                                                m_big_M);

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::Bilevel::KKT::clone() const {
    return new KKT(*this);
}

idol::Bilevel::KKT &idol::Bilevel::KKT::with_big_M(const idol::Annotation<double> &t_big_M) {

    if (m_big_M.has_value()) {
        throw Exception("Big M has already been set.");
    }

    m_big_M = t_big_M;

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
          m_single_level_optimizer(t_src.m_single_level_optimizer->clone()) {

}

idol::Model
idol::Bilevel::KKT::make_model(const idol::Model &t_model, const idol::Bilevel::Description &t_description) {

    if (t_model.get_obj_sense() != Minimize) {
        throw Exception("Only minimization problems are supported.");
    }

    auto& env = t_model.env();

    Reformulators::KKT reformulator(t_model, t_description);

    Model result(env);
    reformulator.add_coupling_variables(result);
    reformulator.add_kkt_reformulation(result);
    reformulator.add_coupling_constraints(result);
    result.set_obj_expr(t_model.get_obj_expr());

    return result;
}

idol::Model idol::Bilevel::KKT::make_model(const idol::Model &t_model,
                                           const idol::Bilevel::Description &t_description,
                                           const idol::Annotation<double> &t_big_M) {

    if (t_model.get_obj_sense() != Minimize) {
        throw Exception("Only minimization problems are supported.");
    }

    auto& env = t_model.env();

    Reformulators::KKT reformulator(t_model, t_description);

    Model result(env);
    reformulator.add_coupling_variables(result);
    reformulator.add_kkt_reformulation(result, t_big_M);
    reformulator.add_coupling_constraints(result);
    result.set_obj_expr(t_model.get_obj_expr());

    return result;
}
