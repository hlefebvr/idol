//
// Created by henri on 27/03/23.
//
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.h"
#include "idol/general/utils/exceptions/Exception.h"

idol::Optimizer *idol::GLPK::operator()(const Model &t_model) const {
    auto* result = new Optimizers::GLPK(t_model, m_continuous_relaxation);
    handle_default_parameters(result);
    return result;
}

idol::GLPK idol::GLPK::ContinuousRelaxation() {
    return GLPK(true);
}

idol::GLPK *idol::GLPK::clone() const {
    return new GLPK(*this);
}

idol::Model idol::GLPK::read_from_file(idol::Env &t_env, const std::string &t_filename) {
    return Optimizers::GLPK::read_from_file(t_env, t_filename);
}
