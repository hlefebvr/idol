//
// Created by henri on 27/03/23.
//
#include "idol/optimizers/mixed-integer-programming/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/HiGHS/Optimizers_HiGHS.h"
#include "idol/errors/Exception.h"

idol::Optimizer *idol::HiGHS::operator()(const Model &t_model) const {
#ifdef IDOL_USE_HIGHS
    auto* result = new Optimizers::HiGHS(t_model, m_continuous_relaxation.has_value() && m_continuous_relaxation.value());

    this->handle_default_parameters(result);

    return result;
#else
    throw Exception("idol was not linked with HiGHS.");
#endif
}

idol::HiGHS idol::HiGHS::ContinuousRelaxation() {
    return HiGHS(true);
}

idol::HiGHS *idol::HiGHS::clone() const {
    return new HiGHS(*this);
}

idol::HiGHS &idol::HiGHS::with_continuous_relaxation_only(bool t_value) {

    if (m_continuous_relaxation.has_value()) {
        throw Exception("Continuous relaxation setting has already been configured.");
    }

    m_continuous_relaxation = t_value;

    return *this;
}
